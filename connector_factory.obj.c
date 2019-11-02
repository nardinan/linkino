/*
 * linkino
 * Copyright (C) 2020 Andrea Nardinocchi (andrea@nardinan.it)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "connector_factory.obj.h"
struct s_connector_factory_attributes *p_connector_factory_alloc(struct s_object *self) {
  struct s_connector_factory_attributes *result = d_prepare(self, connector_factory);
  f_memory_new(self);                                                                 /* inherit */
  f_mutex_new(self);                                                                  /* inherit */
  f_drawable_new(self, (e_drawable_kind_single | e_drawable_kind_force_visibility));  /* inherit */
  f_eventable_new(self);                                                              /* inherit */
  return result;
}
struct s_object *f_connector_factory_new(struct s_object *self, struct s_object *drawable) {
  struct s_connector_factory_attributes *connector_factory_attributes = p_connector_factory_alloc(self);
  if ((connector_factory_attributes->drawable = d_retain(drawable))) {
    connector_factory_attributes->array_of_connectors = f_array_new(d_new(array), d_array_bucket);
  }
  return self;
}
d_define_method_override(connector_factory, event)(struct s_object *self, struct s_object *environment, SDL_Event *current_event) {
  d_using(connector_factory);
  t_boolean changed = d_false;
  if (connector_factory_attributes->drawable) {
    int mouse_x, mouse_y;
    SDL_GetMouseState(&mouse_x, &mouse_y);
    if (connector_factory_attributes->active_connector) {
      /* we might be trigger by two different events:
       * - left click (drop the line where we are, store it into the array of connector and removes the active connector)
       * - right click (removes the active connector)
       * Anyway, before doing anything, we need to update the destination of the active_connector using the current position of the mouse
       */
      d_call(connector_factory_attributes->active_connector, m_connector_set_destination, (double)mouse_x, (double)mouse_y);
      if (current_event->type == SDL_MOUSEBUTTONDOWN) {
        if (current_event->button.button == SDL_BUTTON_LEFT)
          d_call(connector_factory_attributes->array_of_connectors, m_array_push, connector_factory_attributes->active_connector);
        d_delete(connector_factory_attributes->active_connector);
        connector_factory_attributes->active_connector = NULL;
      }
      changed = d_true;
    } else if ((current_event->type == SDL_MOUSEBUTTONDOWN) && (current_event->button.button == SDL_BUTTON_LEFT)) {
      connector_factory_attributes->active_connector =
        f_connector_new(d_new(connector), connector_factory_attributes->drawable, (double)mouse_x, (double)mouse_y);
      changed = d_true;
    }
  }
  d_cast_return(((changed) ? e_eventable_status_captured : e_eventable_status_ignored));
}
d_define_method_override(connector_factory, draw)(struct s_object *self, struct s_object *environment) {
  d_using(connector_factory);
  struct s_environment_attributes *environment_attributes = d_cast(environment, environment);
  struct s_camera_attributes *camera_attributes = d_cast(environment_attributes->current_camera, camera);
  if (connector_factory_attributes->drawable) {
    struct s_object *current_connector;
    if (connector_factory_attributes->active_connector) {
      if ((d_call(connector_factory_attributes->active_connector, m_drawable_normalize_scale, camera_attributes->scene_reference_w,
        camera_attributes->scene_reference_h, camera_attributes->scene_offset_x, camera_attributes->scene_offset_y, camera_attributes->scene_center_x,
        camera_attributes->scene_center_y, camera_attributes->screen_w, camera_attributes->screen_h, camera_attributes->scene_zoom)))
        while (((intptr_t)d_call(connector_factory_attributes->active_connector, m_drawable_draw, environment)) == d_drawable_return_continue);
    }
    d_array_foreach(connector_factory_attributes->array_of_connectors, current_connector) {
      if ((d_call(current_connector, m_drawable_normalize_scale, camera_attributes->scene_reference_w, camera_attributes->scene_reference_h,
        camera_attributes->scene_offset_x, camera_attributes->scene_offset_y, camera_attributes->scene_center_x, camera_attributes->scene_center_y,
        camera_attributes->screen_w, camera_attributes->screen_h, camera_attributes->scene_zoom)))
        while (((intptr_t)d_call(current_connector, m_drawable_draw, environment)) == d_drawable_return_continue);
    }
  }
  d_cast_return(d_drawable_return_last);
}
d_declare_method(connector_factory, delete)(struct s_object *self, struct s_connector_factory_attributes *attributes) {
  if (attributes->array_of_connectors)
    d_delete(attributes->array_of_connectors);
  if (attributes->drawable)
    d_delete(attributes->drawable);
  return NULL;
}
d_define_class(connector_factory) {d_hook_method_override(connector_factory, e_flag_public, eventable, event),
                                   d_hook_method_override(connector_factory, e_flag_public, drawable, draw),
                                   d_hook_delete(connector_factory),
                                   d_hook_method_tail};