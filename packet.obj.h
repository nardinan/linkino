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
#ifndef linkino_packet_h
#define linkino_packet_h
#include "miranda.h"
#include "connector.obj.h"
#define d_packet_maximum_lines 15
#define d_packet_maximum_line_size 256
#define d_packet_direction_first_to_second (1.0)
#define d_packet_direction_second_to_first (-1.0)
#define d_packet_spam         0x0001
#define d_packet_rogue        0x0002
d_declare_class(packet) {
  struct s_attributes head;
  struct s_object *drawable_icon, *drawable_background;
  struct s_object *ui_label_content;
  struct s_object *ui_button_close;
  struct s_object *connector_traveling;
  double traveling_speed, current_position, destination_position, current_direction; // current_position has to be normalized between 0 and 1
  t_boolean analyzing, traveling;
  int flags;
} d_declare_class_tail(packet);
struct s_packet_attributes *p_packet_alloc(struct s_object *self);
extern struct s_object *f_packet_new(struct s_object *self, struct s_object *ui_factory, struct s_object *drawable_icon, 
    struct s_object *drawable_background, const char *body_content, int flags);
d_declare_method(packet, set_traveling)(struct s_object *self, t_boolean traveling, struct s_object *connector_traveling, double current_position,
    double destination_position);
d_declare_method(packet, set_analyzing)(struct s_object *self, t_boolean analyzing);
d_declare_method(packet, set_traveling_speed)(struct s_object *self, double traveling_speed);
d_declare_method(packet, set_direction)(struct s_object *self, double direction);
d_declare_method(packet, is_arrived)(struct s_object *self);
d_declare_method(packet, move_by)(struct s_object *self, double movement);
d_declare_method(packet, event)(struct s_object *self, struct s_object *environment, SDL_Event *current_event);
d_declare_method(packet, draw)(struct s_object *self, struct s_object *environment);
d_declare_method(packet, delete)(struct s_object *self, struct s_packet_attributes *attributes);
#endif
