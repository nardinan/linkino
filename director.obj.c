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
#include "director.obj.h"
struct s_director_attributes *p_director_alloc(struct s_object *self) {
  struct s_director_attributes *result = d_prepare(self, director);
  f_memory_new(self);                                                                 /* inherit */
  f_mutex_new(self);                                                                  /* inherit */
  f_drawable_new(self, (e_drawable_kind_single | e_drawable_kind_force_visibility));  /* inherit */
  f_eventable_new(self);                                                              /* inherit */
  return result;
}
extern struct s_object *f_director_new(struct s_object *self, struct s_object *environment, struct s_object *ui_factory, struct s_object *media_factory) {
  struct s_director_attributes *director_attributes = p_director_alloc(self);
  struct s_object *drawable_line_step;
  director_attributes->environment = d_retain(environment);
  director_attributes->ui_factory = d_retain(ui_factory);
  director_attributes->media_factory = d_retain(media_factory);
  d_assert((drawable_line_step = d_call(media_factory, m_media_factory_get_bitmap, "line_spot")));
  d_assert((director_attributes->connector_factory = f_connector_factory_new(d_new(connector_factory), drawable_line_step)));
  d_assert((director_attributes->connectable_factory = f_connectable_factory_new(d_new(connectable_factory), ui_factory, environment)));
  d_assert((director_attributes->packet_factory = f_packet_factory_new(d_new(packet_factory), director_attributes->ui_factory, 
          director_attributes->media_factory, director_attributes->connectable_factory, director_attributes->connector_factory)));
  return self;
}
d_define_method(director, add_node)(struct s_object *self, const char *stream_icon_label, const char *title, const char *description, double *offsets_x,
                                    double *offsets_y, size_t connections, t_boolean generate_traffic) {
  d_using(director);
  struct s_object *stream;
  struct s_media_factory_attributes *media_factory_attributes = d_cast(director_attributes->media_factory, media_factory);
  if ((stream = d_call(media_factory_attributes->resources_png, m_resources_get_stream_strict, stream_icon_label, e_resources_type_common))) {
    d_call(director_attributes->connectable_factory, m_connectable_factory_add_connectable_template, stream, title, description, offsets_x, offsets_y,
      connections, generate_traffic);
  }
  return self;
}
d_define_method_override(director, event)(struct s_object *self, struct s_object *environment, SDL_Event *current_event) {
  d_using(director);
  t_boolean changed = d_true;
  if (((intptr_t)d_call(director_attributes->connectable_factory, m_eventable_event, environment, current_event)) != e_eventable_status_captured) {
    struct s_object *result = d_call(director_attributes->connectable_factory, m_connectable_factory_get_selected_node, NULL);
    d_call(director_attributes->connector_factory, m_connector_factory_set_drop, ((result) ? d_true : d_false), result);
    if (((intptr_t)d_call(director_attributes->connector_factory, m_eventable_event, environment, current_event)) != e_eventable_status_captured)
      if (((intptr_t)d_call(director_attributes->packet_factory, m_eventable_event, environment, current_event)) != e_eventable_status_captured)
        changed = d_false;
  }
  d_cast_return(((changed) ? e_eventable_status_captured : e_eventable_status_ignored));
}
d_define_method_override(director, draw)(struct s_object *self, struct s_object *environment) {
  d_using(director);
  d_call(director_attributes->connectable_factory, m_drawable_draw, environment);
  d_call(director_attributes->connector_factory, m_drawable_draw, environment);
  d_call(director_attributes->packet_factory, m_drawable_draw, environment);
  d_cast_return(d_drawable_return_last);
}
d_declare_method(director, delete)(struct s_object *self, struct s_director_attributes *attributes) {
  d_delete(attributes->connector_factory);
  d_delete(attributes->connectable_factory);
  d_delete(attributes->packet_factory);
  d_delete(attributes->environment);
  d_delete(attributes->ui_factory);
  d_delete(attributes->media_factory);
  return NULL;
}
d_define_class(director) {d_hook_method(director, e_flag_public, add_node),
  d_hook_method_override(director, e_flag_public, eventable, event),
  d_hook_method_override(director, e_flag_public, drawable, draw),
  d_hook_delete(director),
  d_hook_method_tail};
