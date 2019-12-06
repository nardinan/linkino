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
#define d_connector_increment_weight_per_frame 0.01
#define d_connector_selected_line_distance 4
#define d_connector_maximum_time_snap 4
#define d_connector_minimum_time_snap 2
#define d_connector_overload_limitation 0.75
struct s_connector_segment {
  double position_x, position_y;
  t_boolean initialized;
} s_connector_segment;
d_declare_class(connector) {
  struct s_attributes head;
  struct s_object *starting_point, *destination_point;
  struct s_object *drawable_line;
  double separation, target_weight, current_weight, center_position_x, center_position_y;
  struct s_connectable_link *source_link, *destination_link;
  struct s_connector_segment segments[d_connector_maximum_segments];
  time_t snapping_time, last_timestamp_below_maximum;
} d_declare_class_tail(connector);
struct s_connector_attributes *p_connector_alloc(struct s_object *self);
extern struct s_object *f_connector_new(struct s_object *self, struct s_object *drawable_line, double source_x, 
    double source_y, struct s_connectable_link *link);
d_declare_method(connector, set_starting)(struct s_object *self, double starting_x, double starting_y, struct s_connectable_link *link);
d_declare_method(connector, set_destination)(struct s_object *self, double destination_x, double destination_y, struct s_connectable_link *link);
d_declare_method(connector, set_weight)(struct s_object *self, double current_weight);
d_declare_method(connector, set_separation)(struct s_object *self, double separation);
d_declare_method(connector, get_separation)(struct s_object *self, double *separation);
d_declare_method(connector, get_point)(struct s_object *self, double percentage_path, double *position_x, double *position_y);
d_declare_method(connector, is_over_line)(struct s_object *self, int position_x, int position_y, int line_start_x, int line_start_y, 
    int line_end_x, int line_end_y);
d_declare_method(connector, is_over)(struct s_object *self, int position_x, int position_y);
d_declare_method(connector, is_snapped)(struct s_object *self);
d_declare_method(connector, draw)(struct s_object *self, struct s_object *environment);
d_declare_method(connector, destroy_links)(struct s_object *self);
d_declare_method(connector, delete)(struct s_object *self, struct s_connector_attributes *attributes);
#endif
