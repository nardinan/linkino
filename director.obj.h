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
#define d_director_events 24
#define d_director_stations 15
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
  e_statistics_clock,
  e_statistics_NULL
};
typedef struct s_events_description {
  t_boolean set, triggered, silent_traffic_generators;
  time_t trigger_time, minimum_seconds_between_traffic, maximum_seconds_between_traffic;
  double spam_percentage;
} s_events_description;
typedef struct s_station_description {
  t_boolean set;
  char *title, *unique_code;
  double position_x, position_y;
  struct s_events_description events[d_director_events];
} s_station_description;
typedef struct s_level_description {
  t_boolean set;
  char minimum_class;
  char *background_drawable;
  struct s_station_description stations[d_director_stations];
  struct s_events_description events[d_director_events];
} s_level_events;
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
  struct s_object *ui_clock;
  struct s_uiable_container *ui_labels[e_statistics_NULL];
  struct s_object *drawable_background;
  time_t level_starting_time;
  struct s_level_description current_level;
} d_declare_class_tail(director);
struct s_director_attributes *p_director_alloc(struct s_object *self);
extern struct s_object *f_director_new(struct s_object *self, struct s_object *environment, struct s_object *ui_factory, struct s_object *media_factory);
d_declare_method(director, add_node)(struct s_object *self, const char *stream_icon_label, const char *title, const char *description, double *offsets_x,
    double *offsets_y, size_t connections, double price, int flags);
d_declare_method(director, set_level)(struct s_object *self, struct s_level_description level_description);
d_declare_method(director, update_level)(struct s_object *self);
d_declare_method(director, update_event)(struct s_object *self, struct s_events_description *event, const char *unique_code, time_t seconds_elapsed);
d_declare_method(director, event)(struct s_object *self, struct s_object *environment, SDL_Event *current_event);
d_declare_method(director, draw)(struct s_object *self, struct s_object *environment);
d_declare_method(director, delete)(struct s_object *self, struct s_director_attributes *attributes);
#endif
