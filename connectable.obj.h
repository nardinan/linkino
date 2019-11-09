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
#ifndef linkino_connectable_h
#define linkino_connectable_h
#include "miranda.h"
#define d_connectable_width 10
#define d_connectable_height 10
#define d_connectable_rectangle_elements 4
#define d_connectable_rectangle_R 10
#define d_connectable_rectangle_G 255
#define d_connectable_rectangle_B 10
#define d_connectable_rectangle_A 100
#define d_connectable_code_size 6
struct s_connection_node { d_list_node_head;
  double offset_x, offset_y, width, height, final_position_x, final_position_y;
  char label[d_string_buffer_size], unique_code[d_connectable_code_size];
  struct s_object *connectable;
  t_boolean is_connected;
} s_connection_node;
d_declare_class(connectable) {
  struct s_attributes head;
  struct s_list list_connection_nodes;
  struct s_connection_node *active_node;
  int rectangle_x[d_connectable_rectangle_elements], rectangle_y[d_connectable_rectangle_elements];
  t_boolean draw_rectangle, normalized;
  char unique_code[d_connectable_code_size];
} d_declare_class_tail(connectable);
struct s_connectable_attributes *p_connectable_alloc(struct s_object *self, struct s_object *stream, struct s_object *environment);
extern struct s_object *f_connectable_new(struct s_object *self, struct s_object *stream, struct s_object *environment, const char *unique_code);
d_declare_method(connectable, add_connection_point)(struct s_object *self, double offset_x, double offset_y, const char *label);
d_declare_method(connectable, get_selected_node)(struct s_object *self);
d_declare_method(connectable, event)(struct s_object *self, struct s_object *environment, SDL_Event *current_event);
d_declare_method(connectable, draw)(struct s_object *self, struct s_object *environment);
d_declare_method(connectable, delete)(struct s_object *self, struct s_connectable_attributes *attributes);
#endif