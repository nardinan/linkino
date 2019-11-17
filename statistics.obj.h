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
#ifndef linkino_statistics_h
#define linkino_statistics_h
#include "miranda.h"
d_declare_class(statistics) {
  struct s_attributes head;
  unsigned int packet_not_shipped, packet_lost, packet_shipped, spam_shipped, connector_snapped, sum_packet_hops;
  time_t sum_packet_times;
} d_declare_class_tail(statistics);
struct s_statistics_attributes *p_statistics_alloc(struct s_object *self);
extern struct s_object *f_statistics_new(struct s_object *self);
d_declare_method(statistics, add_packet_not_shipped)(struct s_object *self);
d_declare_method(statistics, add_packet_lost)(struct s_object *self);
d_declare_method(statistics, add_packet_shipped)(struct s_object *self, time_t time, unsigned int hops);
d_declare_method(statistics, add_spam_shipped)(struct s_object *self);
d_declare_method(statistics, add_connector_snapped)(struct s_object *self);
d_declare_method(statistics, get_score)(struct s_object *self, char *character);
d_declare_method(statistics, reset)(struct s_object *self);
#endif
