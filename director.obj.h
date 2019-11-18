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
#ifndef linkino_director_h
#define linkino_director_h
#include "connector_factory.obj.h"
#include "connectable_factory.obj.h"
#include "packet_factory.obj.h"
#include "statistics.obj.h"
enum e_director_statistics_label {
  e_statistics_packet_shipped = 0,
  e_statistics_packet_lost,
  e_statistics_spam,
  e_statistics_average_time,
  e_statistics_average_hops,
  e_statistics_score,
  e_statistics_NULL
};
d_declare_class(director) {
  struct s_attributes head;
  struct s_object *environment;
  struct s_object *ui_factory;
  struct s_object *media_factory;
  struct s_object *connectable_factory;
  struct s_object *connector_factory;
  struct s_object *packet_factory;
  struct s_object *statistics;
  struct s_object *ui_statistics;
  struct s_uiable_container *ui_labels[e_statistics_NULL];
  unsigned int current_credit;
} d_declare_class_tail(director);
struct s_director_attributes *p_director_alloc(struct s_object *self);
extern struct s_object *f_director_new(struct s_object *self, struct s_object *environment, struct s_object *ui_factory, struct s_object *media_factory);
d_declare_method(director, add_node)(struct s_object *self, const char *stream_icon_label, const char *title, const char *description, double *offsets_x,
  double *offsets_y, size_t connections, double price, t_boolean generate_traffic, t_boolean filter_spam);
d_declare_method(director, event)(struct s_object *self, struct s_object *environment, SDL_Event *current_event);
d_declare_method(director, draw)(struct s_object *self, struct s_object *environment);
d_declare_method(director, delete)(struct s_object *self, struct s_director_attributes *attributes);
#endif
