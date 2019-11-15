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
#include "statistics.obj.h"
struct s_statistics_attributes *p_statistics_alloc(struct s_object *self) {
  struct s_statistics_attributes *result = d_prepare(self, statistics);
  f_memory_new(self);   /* inherit */
  f_mutex_new(self);    /* inherit */
  return result;
}
struct s_object *f_statistics_new(struct s_object *self) {
  struct s_statistics_attributes *statistics_attributes = p_statistics_alloc(self);
  return self;
}
d_define_method(statistics, add_packet_not_shipped)(struct s_object *self) {
  d_using(statistics);
  ++(statistics_attributes->packet_not_shipped);
  return self;
}
d_define_method(statistics, add_packet_lost)(struct s_object *self) {
  d_using(statistics);
  ++(statistics_attributes->packet_lost);
  return self;
}
d_define_method(statistics, add_packet_shipped)(struct s_object *self, time_t time, unsigned int hops){ 
  d_using(statistics);
  ++(statistics_attributes->packet_shipped);
  statistics_attributes->sum_packet_times += time;
  statistics_attributes->sum_packet_hops += hops;
  return self;
}
d_define_method(statistics, add_spam_shipped)(struct s_object *self) {
  d_using(statistics);
  ++(statistics_attributes->spam_shipped);
  return self;
}
d_define_method(statistics, add_connector_snapped)(struct s_object *self) {
  d_using(statistics);
  ++(statistics_attributes->connector_snapped);
  return self;
}
d_define_class(statistics) { d_hook_method(statistics, e_flag_public, add_packet_not_shipped),
  d_hook_method(statistics, e_flag_public, add_packet_lost),
  d_hook_method(statistics, e_flag_public, add_packet_shipped),
  d_hook_method(statistics, e_flag_public, add_spam_shipped),
  d_hook_method(statistics, e_flag_public, add_connector_snapped),
  d_hook_method_tail
};
