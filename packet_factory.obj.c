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
#include "packet_factory.obj.h"
struct s_packet_factory_attributes *p_packet_factory_alloc(struct s_object *self) {
  struct s_packet_factory_attributes *result = d_prepare(self, packet_factory);
  f_memory_new(self);                                                                 /* inherit */
  f_mutex_new(self);                                                                  /* inherit */
  f_drawable_new(self, (e_drawable_kind_single | e_drawable_kind_force_visibility));  /* inherit */
  f_eventable_new(self);                                                              /* inherit */
  return result;
}
struct s_object *f_packet_factory_new(struct s_object *self, struct s_object *ui_factory, struct s_object *media_factory, 
    struct s_object *connectable_factory) {
  struct s_packet_factory_attributes *packet_factory_attributes = p_packet_factory_alloc(self);
  packet_factory_attributes->ui_factory = d_retain(ui_factory);
  packet_factory_attributes->media_factory = d_retain(media_factory);
  packet_factory_attributes->connectable_factory = d_retain(connectable_factory);
  d_assert(packet_factory_attributes->array_packets = f_array_new(d_new(array), d_array_bucket));
  memset(&(packet_factory_attributes->statistics_generated), 0, sizeof(struct s_list));
  return self;
}
d_define_method_override(packet_factory, event)(struct s_object *self, struct s_object *environment, SDL_Event *current_event) {
  t_boolean changed = d_false;
  d_cast_return(((changed) ? e_eventable_status_captured : e_eventable_status_ignored));
}
d_define_method_override(packet_factory, draw)(struct s_object *self, struct s_object *environment) {
  d_cast_return(d_drawable_return_last);
}
d_define_method(packet_factory, delete)(struct s_object *self, struct s_packet_factory_attributes *attributes) {
  struct s_packet_factory_statistics *current_statistics;
  d_delete(attributes->ui_factory);
  d_delete(attributes->media_factory);
  d_delete(attributes->connectable_factory);
  d_delete(attributes->array_packets);
  while ((current_statistics = (struct s_packet_factory_statistics *)(attributes->statistics_generated.head))) {
    f_list_delete(&(attributes->statistics_generated), (struct s_list_node *)current_statistics);
    d_free(current_statistics);
  }
  return NULL;
}
d_define_class(packet_factory) {d_hook_method_override(packet_factory, e_flag_public, eventable, event),
                                d_hook_method_override(packet_factory, e_flag_public, drawable, draw),
                                d_hook_delete(packet_factory)};
