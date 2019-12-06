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
#define d_connectable_rectangle_used_R 255
#define d_connectable_rectangle_used_G 10
#define d_connectable_rectangle_used_B 10
#define d_connectable_rectangle_used_A 100
#define d_connectable_code_size 10
#define d_connectable_min_seconds_between_generation 1
#define d_connectable_max_seconds_between_generation 10
#define d_connectable_max_packets 64
/* set of flags that defines the behavior of the device */
#define d_connectable_shape_traffic      0x0001
#define d_connectable_block_spam         0x0002
#define d_connectable_accelerate_medium  0x0004
#define d_connectable_accelerate_alot    0x0008
#define d_connectable_generate_traffic   0x0010
#define d_connectable_can_be_acquired    0x0020
#define d_connectable_cannot_be_moved    0x0040
#define d_connectable_slow_down_traffic  0x0080
#define d_connectable_refresh_expiration 0x0100
/* end of flags */
extern unsigned int index_human_name;
extern const char *list_human_names[];
struct s_connectable_link { d_list_node_head;
  double offset_x, offset_y, width, height, final_position_x, final_position_y;
  char label[d_string_buffer_size], unique_code[d_connectable_code_size];
  struct s_object *connectable;
  struct s_object *connector;
  struct s_object *traveling_packets[d_connectable_max_packets];
} s_connectable_link;
d_declare_class(connectable) {
  struct s_attributes head;
  struct s_list list_connection_nodes;
  struct s_connectable_link *active_node;
  struct s_object *ui_label;
  int rectangle_x[d_connectable_rectangle_elements], rectangle_y[d_connectable_rectangle_elements], flags;
  t_boolean draw_rectangle, used_rectangle, normalized, silent;
  time_t next_token_generation, seconds_between_generation_minimum, seconds_between_generation_maximum;
  char unique_code[d_connectable_code_size];
  double price, spam_percentage;
} d_declare_class_tail(connectable);
struct s_connectable_attributes *p_connectable_alloc(struct s_object *self, struct s_object *stream, struct s_object *environment);
extern struct s_object *f_connectable_new(struct s_object *self, struct s_object *stream, struct s_object *environment, struct s_object *ui_factory,
    const char *unique_code, int flags);
d_declare_method(connectable, set_generate_traffic_speed)(struct s_object *self, time_t minimum_seconds_between_traffic,
    time_t maximum_seconds_between_traffic);
d_declare_method(connectable, set_silent)(struct s_object *self, t_boolean silent);
d_declare_method(connectable, set_spam_percentage)(struct s_object *self, double spam_percentage);
d_declare_method(connectable, set_price)(struct s_object *self, double price);
d_declare_method(connectable, add_connection_point)(struct s_object *self, double offset_x, double offset_y, const char *label);
d_declare_method(connectable, get_selected_node)(struct s_object *self);
d_declare_method(connectable, is_traffic_generation_required)(struct s_object *self);
d_declare_method(connectable, is_over)(struct s_object *self, int mouse_x, int mouse_y);
d_declare_method(connectable, event)(struct s_object *self, struct s_object *environment, SDL_Event *current_event);
d_declare_method(connectable, draw)(struct s_object *self, struct s_object *environment);
d_declare_method(connectable, delete)(struct s_object *self, struct s_connectable_attributes *attributes);
#endif
