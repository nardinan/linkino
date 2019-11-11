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
#ifndef linkino_packet_factory_h
#define linkino_packet_factory_h
#include "connectable_factory.obj.h"
#define d_packet_factory_statistics_maximum_targets 10
typedef struct s_packet_factory_statistics { d_list_node_head;
  struct s_object *connectable;
  double probability_target[d_packet_factory_statistics_maximum_targets]; /* the sum of these values has to be 1 */
} s_packet_factory_statistics;
d_declare_class(packet_factory) {
  struct s_attributes head;
  struct s_object *ui_factory;
  struct s_object *media_factory;
  struct s_object *connectable_factory;
  struct s_object *array_packets;
  struct s_list statistics_generated;
} d_declare_class_tail(packet_factory);
struct s_packet_factory_attributes *p_packet_factory_alloc(struct s_object *self);
extern struct s_object *f_packet_factory_new(struct s_object *self, struct s_object *ui_factory, struct s_object *media_factory,
    struct s_object *connectable_factory);
d_declare_method(packet_factory, event)(struct s_object *self, struct s_object *environment, SDL_Event *current_event);
d_declare_method(packet_factory, draw)(struct s_object *self, struct s_object *environment);
d_declare_method(packet_factory, delete)(struct s_object *self, struct s_packet_factory_attributes *attributes);
#endif
