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
#define d_connectable_width 50
#define d_connectable_height 50
struct s_connection_node { d_list_node_head;
  double offset_x, offset_y, width, height;
  char label[d_string_buffer_size];
} s_connection_node;
d_declare_class(connectable) {
  struct s_attributes head;
  struct s_list list_connection_nodes;
  struct s_connection_node *selected_node;
} d_declare_class_tail(connectable);
struct s_connectable_attributes *p_connectable_alloc(struct s_object *self);
extern struct s_object *f_connectable_new(struct s_object *self);
d_declare_method(connectable, add_connection_point)(struct s_object *self, double offset_x, double offset_y, const char *label);
d_declare_method(connectable, get_selected_node)(struct s_object *self);
d_declare_method(connectable, event)(struct s_object *self, struct s_object *environment, SDL_Event *current_event);
d_declare_method(connectable, delete)(struct s_object *self, struct s_connectable_attributes *attributes);
#endif