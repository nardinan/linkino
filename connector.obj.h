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
#ifndef linkino_connector_h
#define linkino_connector_h
#include "miranda.h"
#include "connectable.obj.h"
#define d_connector_maximum_segments 10
struct s_connector_segment {
  double position_x, position_y;
  t_boolean initialized;
} s_connector_segment;
d_declare_class(connector) {
  struct s_attributes head;
  struct s_object *starting_point, *destination_point;
  struct s_object *drawable;
  struct s_object *array_packets;
  double separation;
  struct s_connectable_link *source_link, *destination_link;
  struct s_connector_segment segments[d_connector_maximum_segments];
} d_declare_class_tail(connector);
struct s_connector_attributes *p_connector_alloc(struct s_object *self);
extern struct s_object *f_connector_new(struct s_object *self, struct s_object *drawable, double source_x, double source_y, struct s_connectable_link *link);
d_declare_method(connector, set_starting)(struct s_object *self, double starting_x, double starting_y, struct s_connectable_link *link);
d_declare_method(connector, set_destination)(struct s_object *self, double destination_x, double destination_y, struct s_connectable_link *link);
d_declare_method(connector, get_point)(struct s_object *self, double percentage_path, double *position_x, double *position_y);
d_declare_method(connector, draw)(struct s_object *self, struct s_object *environment);
d_declare_method(connector, delete)(struct s_object *self, struct s_connector_attributes *attributes);
#endif
