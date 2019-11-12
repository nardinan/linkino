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
#include "connectable_factory.obj.h"
void p_connectable_factory_left_click(struct s_object *self, void **parameters, size_t entries) {
  struct s_object *connectable_factory = (struct s_object *)parameters[0];
  struct s_connectable_factory_template *current_template = (struct s_connectable_factory_template *)parameters[1];
  d_call(connectable_factory, m_connectable_factory_click_received, current_template);
}
struct s_connectable_factory_attributes *p_connectable_factory_alloc(struct s_object *self) {
  struct s_connectable_factory_attributes *result = d_prepare(self, connectable_factory);
  f_memory_new(self);                                                                 /* inherit */
  f_mutex_new(self);                                                                  /* inherit */
  f_drawable_new(self, (e_drawable_kind_single | e_drawable_kind_force_visibility));  /* inherit */
  f_eventable_new(self);                                                              /* inherit */
  return result;
}
struct s_object *f_connectable_factory_new(struct s_object *self, struct s_object *ui_factory, struct s_object *environment) {
  struct s_connectable_factory_attributes *connectable_factory_attributes = p_connectable_factory_alloc(self);
  d_assert(connectable_factory_attributes->array_connectable_instances = f_array_new(d_new(array), d_array_bucket));
  connectable_factory_attributes->ui_factory = d_retain(ui_factory);
  connectable_factory_attributes->environment = d_retain(environment);
  memset(&(connectable_factory_attributes->list_templates), 0, sizeof(struct s_list));
  return self;
}
d_define_method(connectable_factory, add_connectable_template)(struct s_object *self, struct s_object *stream, const char *title, const char *description,
    double *offsets_x, double *offsets_y, size_t connections, t_boolean generate_traffic) {
  d_using(connectable_factory);
  if (connections <= d_connectable_factory_connections) {
    struct s_connectable_factory_template *current_template;
    if ((current_template = (struct s_connectable_factory_template *)d_malloc(sizeof(struct s_connectable_factory_template)))) {
      current_template->stream = d_retain(stream);
      strncpy(current_template->title, title, d_string_buffer_size);
      strncpy(current_template->description, description, d_string_buffer_size);
      memcpy(current_template->offsets_x, offsets_x, (sizeof(double) * connections));
      memcpy(current_template->offsets_y, offsets_y, (sizeof(double) * connections));
      current_template->generate_traffic = generate_traffic;
      current_template->connections = connections;
      if ((current_template->uiable_button =
             d_call(connectable_factory_attributes->ui_factory, m_ui_factory_new_button, d_ui_factory_default_font_id, d_ui_factory_default_font_style,
                    current_template->title))) {
        d_call(current_template->uiable_button, m_emitter_embed_parameter, "clicked_left", self);
        d_call(current_template->uiable_button, m_emitter_embed_parameter, "clicked_left", current_template);
        d_call(current_template->uiable_button, m_emitter_embed_function, "clicked_left", p_connectable_factory_left_click);
        d_call(current_template->uiable_button, m_drawable_set_dimension, d_connectable_factory_button_width, d_connectable_factory_button_height);
        if ((current_template->drawable_icon = f_bitmap_new(d_new(bitmap), current_template->stream, connectable_factory_attributes->environment))) {
          f_list_append(&(connectable_factory_attributes->list_templates), (struct s_list_node *)current_template, e_list_insert_tail);
        } else
          d_die(d_error_malloc);
      } else
        d_die(d_error_malloc);
    } else
      d_die(d_error_malloc);
  }
  return self;
}
d_define_method(connectable_factory, get_selected_node)(struct s_object *self) {
  d_using(connectable_factory);
  struct s_connectable_link *result = connectable_factory_attributes->active_node;
  /* the routine takes the latest selected node, then it resets the pointer to NULL in order to be ready for the next release */
  connectable_factory_attributes->active_node = NULL;
  d_cast_return(result);
}
d_define_method(connectable_factory, is_traffic_generation_required)(struct s_object *self) {
  d_using(connectable_factory);
  struct s_object *current_connectable;
  struct s_connectable_link *result = NULL;
  d_array_foreach(connectable_factory_attributes->array_connectable_instances, current_connectable) {
    if ((result = (struct s_connectable_link *)d_call(current_connectable, m_connectable_is_traffic_generation_required, NULL))) {
      break;
    }
  }
  d_cast_return(result);
}
d_define_method(connectable_factory, click_received)(struct s_object *self, struct s_connectable_factory_template *template) {
  d_using(connectable_factory);
  if (!connectable_factory_attributes->active_template)
    connectable_factory_attributes->active_template = template;
  return self;
}
d_define_method_override(connectable_factory, event)(struct s_object *self, struct s_object *environment, SDL_Event *current_event) {
  d_using(connectable_factory);
  struct s_object *current_connectable;
  struct s_connectable_factory_template *current_template;
  char buffer[d_string_buffer_size];
  t_boolean changed = d_false;
  if (connectable_factory_attributes->active_template) {
    if ((current_event->type == SDL_MOUSEBUTTONDOWN) && (current_event->button.button == SDL_BUTTON_LEFT)) {
      /* we generate a random string that will be used to identify univocally the instance */
      /* we drop the active template and we create a new connectable that we push into the array */
      struct s_object *connectable =
        f_connectable_new(d_new(connectable), connectable_factory_attributes->active_template->stream, 
            connectable_factory_attributes->environment, ((connectable_factory_attributes->active_template->generate_traffic)?d_true:d_false));
      d_call(connectable, m_connectable_set_generate_traffic, connectable_factory_attributes->active_template->generate_traffic);
      d_call(connectable, m_drawable_set_position, connectable_factory_attributes->active_template->position_x,
             connectable_factory_attributes->active_template->position_y);
      for (size_t index_offset = 0; index_offset < connectable_factory_attributes->active_template->connections; ++index_offset) {
        snprintf(buffer, d_string_buffer_size, "%c", (char)(((char)'A') + index_offset));
        d_call(connectable, m_connectable_add_connection_point, connectable_factory_attributes->active_template->offsets_x[index_offset],
               connectable_factory_attributes->active_template->offsets_y[index_offset], buffer);
      }
      d_call(connectable_factory_attributes->array_connectable_instances, m_array_push, connectable);
      connectable_factory_attributes->active_template = NULL;
    }
    changed = d_true;
  } else {
    d_array_foreach(connectable_factory_attributes->array_connectable_instances, current_connectable)
      if (((intptr_t)d_call(current_connectable, m_eventable_event, environment, current_event)) == e_eventable_status_captured) {
        struct s_connectable_link *connection_node;
        if ((connection_node = d_call(current_connectable, m_connectable_get_selected_node, NULL))) {
          connectable_factory_attributes->active_node = connection_node;
          break;
        }
      }
  }
  d_foreach(&(connectable_factory_attributes->list_templates), current_template, struct s_connectable_factory_template)
    d_call_owner(current_template->uiable_button, uiable, m_eventable_event, environment, current_event);
  /* we need to consider what happens if the button is clicked (so we have an active template) */
  if (connectable_factory_attributes->active_template)
    changed = d_true;
  d_cast_return(((changed) ? e_eventable_status_captured : e_eventable_status_ignored));
}
d_define_method_override(connectable_factory, draw)(struct s_object *self, struct s_object *environment) {
  d_using(connectable_factory);
  struct s_environment_attributes *environment_attributes = d_cast(environment, environment);
  struct s_camera_attributes *camera_attributes = d_cast(environment_attributes->current_camera, camera);
  struct s_connectable_factory_template *current_template;
  struct s_object *current_connectable;
  double position_y = 0, position_x = (camera_attributes->scene_reference_w - d_connectable_factory_button_width - 1);
  d_foreach(&(connectable_factory_attributes->list_templates), current_template, struct s_connectable_factory_template) {
    d_call(current_template->uiable_button, m_drawable_set_position, position_x, position_y);
    if ((d_call(current_template->uiable_button, m_drawable_normalize_scale, camera_attributes->scene_reference_w, camera_attributes->scene_reference_h,
                camera_attributes->scene_offset_x, camera_attributes->scene_offset_y, camera_attributes->scene_center_x, camera_attributes->scene_center_y,
                camera_attributes->screen_w, camera_attributes->screen_h, camera_attributes->scene_zoom)))
      while (((intptr_t)d_call(current_template->uiable_button, m_drawable_draw, environment)) == d_drawable_return_continue);
    position_y += (d_connectable_factory_button_height + 1);
  }
  d_array_foreach(connectable_factory_attributes->array_connectable_instances, current_connectable) {
    if ((d_call(current_connectable, m_drawable_normalize_scale, camera_attributes->scene_reference_w, camera_attributes->scene_reference_h,
                camera_attributes->scene_offset_x, camera_attributes->scene_offset_y, camera_attributes->scene_center_x, camera_attributes->scene_center_y,
                camera_attributes->screen_w, camera_attributes->screen_h, camera_attributes->scene_zoom)))
      while (((intptr_t)d_call(current_connectable, m_drawable_draw, environment)) == d_drawable_return_continue);
  }
  /* we have something selected, so we need to keep the icon floating around following the mouse */
  if (connectable_factory_attributes->active_template) {
    double icon_width, icon_height;
    int mouse_x, mouse_y;
    d_call(environment, m_environment_get_mouse_normalized, "draw_camera", &mouse_x, &mouse_y);
    d_call(connectable_factory_attributes->active_template->drawable_icon, m_drawable_get_dimension, &icon_width, &icon_height);
    connectable_factory_attributes->active_template->position_x = mouse_x - (icon_width / 2.0);
    connectable_factory_attributes->active_template->position_y = mouse_y - (icon_height / 2.0);
    d_call(connectable_factory_attributes->active_template->drawable_icon, m_drawable_set_position, connectable_factory_attributes->active_template->position_x,
           connectable_factory_attributes->active_template->position_y);
    d_call(connectable_factory_attributes->active_template->drawable_icon, m_drawable_set_maskA, d_connectable_factory_alpha);
    if ((d_call(connectable_factory_attributes->active_template->drawable_icon, m_drawable_normalize_scale, camera_attributes->scene_reference_w,
                camera_attributes->scene_reference_h, camera_attributes->scene_offset_x, camera_attributes->scene_offset_y, camera_attributes->scene_center_x,
                camera_attributes->scene_center_y, camera_attributes->screen_w, camera_attributes->screen_h, camera_attributes->scene_zoom)))
      while (((intptr_t)d_call(connectable_factory_attributes->active_template->drawable_icon, m_drawable_draw, environment)) == d_drawable_return_continue);
  }
  d_cast_return(d_drawable_return_last);
}
d_define_method(connectable_factory, delete)(struct s_object *self, struct s_connectable_factory_attributes *attributes) {
  struct s_connectable_factory_template *current_template;
  while ((current_template = (struct s_connectable_factory_template *)attributes->list_templates.head)) {
    d_delete(current_template->uiable_button);
    d_delete(current_template->drawable_icon);
    d_free(current_template);
    f_list_delete(&(attributes->list_templates), (struct s_list_node *)attributes->list_templates.head);
  }
  return NULL;
}
d_define_class(connectable_factory) {d_hook_method(connectable_factory, e_flag_public, add_connectable_template),
                                     d_hook_method(connectable_factory, e_flag_public, get_selected_node),
                                     d_hook_method(connectable_factory, e_flag_public, is_traffic_generation_required),
                                     d_hook_method(connectable_factory, e_flag_public, click_received),
                                     d_hook_method_override(connectable_factory, e_flag_public, eventable, event),
                                     d_hook_method_override(connectable_factory, e_flag_public, drawable, draw),
                                     d_hook_delete(connectable_factory),
                                     d_hook_method_tail};
