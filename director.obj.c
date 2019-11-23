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
#include "director.obj.h"
struct s_director_attributes *p_director_alloc(struct s_object *self) {
  struct s_director_attributes *result = d_prepare(self, director);
  f_memory_new(self);                                                                 /* inherit */
  f_mutex_new(self);                                                                  /* inherit */
  f_drawable_new(self, (e_drawable_kind_single | e_drawable_kind_force_visibility));  /* inherit */
  f_eventable_new(self);                                                              /* inherit */
  return result;
}
extern struct s_object *f_director_new(struct s_object *self, struct s_object *environment, struct s_object *ui_factory, struct s_object *media_factory) {
  struct s_director_attributes *director_attributes = p_director_alloc(self);
  struct s_object *drawable_line_step;
  struct s_uiable_container *container;
  director_attributes->environment = d_retain(environment);
  director_attributes->ui_factory = d_retain(ui_factory);
  director_attributes->media_factory = d_retain(media_factory);
  d_assert((drawable_line_step = d_call(media_factory, m_media_factory_get_bitmap, "line_spot")));
  d_assert((director_attributes->statistics = f_statistics_new(d_new(statistics))));
  d_assert((director_attributes->connector_factory = f_connector_factory_new(d_new(connector_factory), drawable_line_step,
          director_attributes->statistics)));
  d_assert((director_attributes->connectable_factory = f_connectable_factory_new(d_new(connectable_factory), ui_factory, environment)));
  d_assert((director_attributes->packet_factory = f_packet_factory_new(d_new(packet_factory), director_attributes->ui_factory, 
          director_attributes->media_factory, director_attributes->connectable_factory, director_attributes->connector_factory,
          director_attributes->statistics)));
  if ((container = d_call(director_attributes->ui_factory, m_ui_factory_get_component, NULL, "statistics"))) {
    director_attributes->ui_statistics = container->uiable;
    d_assert((director_attributes->ui_labels[e_statistics_packet_shipped] = d_call(director_attributes->ui_factory, m_ui_factory_get_component, 
            container, "result_label1")));
    d_assert((director_attributes->ui_labels[e_statistics_packet_lost] = d_call(director_attributes->ui_factory, m_ui_factory_get_component, 
            container, "result_label2")));
    d_assert((director_attributes->ui_labels[e_statistics_spam] = d_call(director_attributes->ui_factory, m_ui_factory_get_component, 
            container, "result_label3")));
    d_assert((director_attributes->ui_labels[e_statistics_average_time] = d_call(director_attributes->ui_factory, m_ui_factory_get_component, 
            container, "result_label4")));
    d_assert((director_attributes->ui_labels[e_statistics_average_hops] = d_call(director_attributes->ui_factory, m_ui_factory_get_component, 
            container, "result_label5")));
    d_assert((director_attributes->ui_labels[e_statistics_score] = d_call(director_attributes->ui_factory, m_ui_factory_get_component,
            container, "score_label")));
    d_call(director_attributes->environment, m_environment_add_drawable, director_attributes->ui_statistics, (d_ui_factory_default_level + 2),
        e_environment_surface_primary);
    if ((container = d_call(director_attributes->ui_factory, m_ui_factory_get_component, NULL, "clock"))) {
      director_attributes->ui_clock = container->uiable;
      d_assert((director_attributes->ui_labels[e_statistics_clock] = d_call(director_attributes->ui_factory, m_ui_factory_get_component,
              container, "clock_label")));
      d_call(director_attributes->environment, m_environment_add_drawable, director_attributes->ui_clock, (d_ui_factory_default_level + 2),
          e_environment_surface_primary);
    }
  }
  d_call(director_attributes->connectable_factory, m_connectable_factory_set_credit, 12000.0);
  return self;
}
d_define_method(director, add_node)(struct s_object *self, const char *stream_icon_label, const char *title, const char *description, double *offsets_x,
    double *offsets_y, size_t connections, double price, int flags) {
  d_using(director);
  struct s_object *stream;
  struct s_media_factory_attributes *media_factory_attributes = d_cast(director_attributes->media_factory, media_factory);
  if ((stream = d_call(media_factory_attributes->resources_png, m_resources_get_stream_strict, stream_icon_label, e_resources_type_common))) {
    d_call(director_attributes->connectable_factory, m_connectable_factory_add_connectable_template, stream, title, description, offsets_x, offsets_y,
        connections, price, flags);
  }
  return self;
}
d_define_method(director, set_level)(struct s_object *self, struct s_level_description level_description) {
  d_using(director);
  enum e_media_factory_media_types current_type;
  director_attributes->current_level = level_description;
  /* we might want to reset the environment, and to generate the stations */
  d_call(director_attributes->statistics, m_statistics_reset, NULL);
  d_call(director_attributes->packet_factory, m_packet_factory_reset, NULL);
  d_call(director_attributes->connector_factory, m_connector_factory_reset, NULL);
  d_call(director_attributes->connectable_factory, m_connectable_factory_reset, NULL);
  if (director_attributes->drawable_background) {
    d_call(director_attributes->environment, m_environment_del_drawable, director_attributes->drawable_background, 
        d_ui_factory_default_level, e_environment_surface_primary);
    d_delete(director_attributes->drawable_background);
  }
  if ((director_attributes->drawable_background = d_call(director_attributes->media_factory, m_media_factory_get_media, 
          level_description.background_drawable, &current_type)))
    d_call(director_attributes->environment, m_environment_add_drawable, director_attributes->drawable_background,
        d_ui_factory_default_level, e_environment_surface_primary);
  for (size_t index = 0; index < d_director_stations; ++index)
    if (level_description.stations[index].set) {
      d_call(director_attributes->connectable_factory, m_connectable_factory_add_connectable_instance, 
          level_description.stations[index].title,
          level_description.stations[index].unique_code,
          level_description.stations[index].position_x,
          level_description.stations[index].position_y);
    }

  d_call(director_attributes->connectable_factory, m_connectable_factory_set_silent, d_true, NULL);
  director_attributes->level_starting_time = time(NULL);
  return self;
}
d_define_method(director, update_level)(struct s_object *self) {
  d_using(director);
  if (director_attributes->current_level.set) {
    time_t seconds_elapsed = time(NULL) - director_attributes->level_starting_time, current_hour, 
           current_minutes;
    char buffer[d_string_buffer_size];
    current_hour = 7 + (seconds_elapsed / 60);
    current_minutes = (seconds_elapsed % 60);
    snprintf(buffer, d_string_buffer_size, "%02zu:%02zu", current_hour, current_minutes);
    d_call(director_attributes->ui_labels[e_statistics_clock]->uiable, m_label_set_content_char, buffer, NULL, director_attributes->environment);
    for (size_t index = 0; index < d_director_events; ++index)
      if (director_attributes->current_level.events[index].set) {
        d_call(self, m_director_update_event, &(director_attributes->current_level.events[index]), NULL, seconds_elapsed);
      } else
        break;
    for (size_t index_station = 0; index_station < d_director_stations; ++index_station)
      if (director_attributes->current_level.stations[index_station].set) {
        for (size_t index_event = 0; index_event < d_director_events; ++index_event)
          if (director_attributes->current_level.stations[index_station].events[index_event].set) {
            d_call(self, m_director_update_event, &(director_attributes->current_level.stations[index_station].events[index_event]),
                director_attributes->current_level.stations[index_station].unique_code, seconds_elapsed);
          } else
            break;
      } else
        break;
  }
  return self;
}
d_define_method(director, update_event)(struct s_object *self, struct s_events_description *event, const char *unique_code, time_t seconds_elapsed) {
  d_using(director);
  if ((event->set) && (!event->triggered))
    if (event->trigger_time < seconds_elapsed) {
      d_call(director_attributes->connectable_factory, m_connectable_factory_set_generate_traffic_speed, event->minimum_seconds_between_traffic,
          event->maximum_seconds_between_traffic, unique_code);
      d_call(director_attributes->connectable_factory, m_connectable_factory_set_silent, event->silent_traffic_generators, unique_code);
      d_call(director_attributes->connectable_factory, m_connectable_factory_set_spam_percentage, event->spam_percentage, unique_code);
      event->triggered = d_true;
    }
  return self;
}
d_define_method_override(director, event)(struct s_object *self, struct s_object *environment, SDL_Event *current_event) {
  d_using(director);
  t_boolean changed = d_true, connector_selected = d_false;
  if ((current_event->type == SDL_KEYDOWN) && (current_event->key.keysym.sym == SDLK_TAB)) {
    d_call(director_attributes->statistics, m_statistics_reset, NULL);
    d_call(director_attributes->packet_factory, m_packet_factory_reset, NULL);
    d_call(director_attributes->connector_factory, m_connector_factory_reset, NULL);
    d_call(director_attributes->connectable_factory, m_connectable_factory_reset, NULL);
  }
  if (((intptr_t)d_call(director_attributes->connectable_factory, m_eventable_event, environment, current_event)) != e_eventable_status_captured) {
    struct s_object *result = d_call(director_attributes->connectable_factory, m_connectable_factory_get_selected_node, NULL);
    d_call(director_attributes->connector_factory, m_connector_factory_set_drop, ((result) ? d_true : d_false), result);
    if (((intptr_t)d_call(director_attributes->connector_factory, m_eventable_event, environment, current_event)) != e_eventable_status_captured) {
      if (((intptr_t)d_call(director_attributes->packet_factory, m_eventable_event, environment, current_event)) != e_eventable_status_captured)
        changed = d_false;
    } else
      connector_selected = d_true;
  }
  d_call(director_attributes->connectable_factory, m_connectable_factory_set_connector_selected, connector_selected);
  d_cast_return(((changed) ? e_eventable_status_captured : e_eventable_status_ignored));
}
d_define_method_override(director, draw)(struct s_object *self, struct s_object *environment) {
  d_using(director);
  struct s_statistics_attributes *statistics_attributes = d_cast(director_attributes->statistics, statistics);
  char buffer[d_string_buffer_size], score = '*';
  d_call(self, m_director_update_level, NULL);
  d_call(director_attributes->connectable_factory, m_drawable_draw, environment);
  d_call(director_attributes->connector_factory, m_drawable_draw, environment);
  d_call(director_attributes->packet_factory, m_drawable_draw, environment);
  /* we do the update of the statistics */
  snprintf(buffer, d_string_buffer_size, "%d", statistics_attributes->packet_shipped);
  d_call(director_attributes->ui_labels[e_statistics_packet_shipped]->uiable, m_label_set_content_char, buffer, NULL, director_attributes->environment);
  snprintf(buffer, d_string_buffer_size, "%d", (statistics_attributes->packet_lost + statistics_attributes->packet_not_shipped));
  d_call(director_attributes->ui_labels[e_statistics_packet_lost]->uiable, m_label_set_content_char, buffer, NULL, director_attributes->environment);
  snprintf(buffer, d_string_buffer_size, "%d", statistics_attributes->spam_shipped);
  d_call(director_attributes->ui_labels[e_statistics_spam]->uiable, m_label_set_content_char, buffer, NULL, director_attributes->environment);
  if (statistics_attributes->sum_packet_times > 0)
    snprintf(buffer, d_string_buffer_size, "%.02f", (((double)statistics_attributes->sum_packet_times)/((double)statistics_attributes->packet_shipped)));
  else
    strncpy(buffer, "0.0", d_string_buffer_size);
  d_call(director_attributes->ui_labels[e_statistics_average_time]->uiable, m_label_set_content_char, buffer, NULL, director_attributes->environment);
  if (statistics_attributes->sum_packet_times > 0)
    snprintf(buffer, d_string_buffer_size, "%.02f", (((double)statistics_attributes->sum_packet_hops)/((double)statistics_attributes->packet_shipped)));
  else
    strncpy(buffer, "0.0", d_string_buffer_size);
  d_call(director_attributes->ui_labels[e_statistics_average_hops]->uiable, m_label_set_content_char, buffer, NULL, director_attributes->environment);
  d_call(director_attributes->statistics, m_statistics_get_score, &score);
  snprintf(buffer, d_string_buffer_size, "class %c", score);
  d_call(director_attributes->ui_labels[e_statistics_score]->uiable, m_label_set_content_char, buffer, NULL, director_attributes->environment);
  d_cast_return(d_drawable_return_last);
}
d_declare_method(director, delete)(struct s_object *self, struct s_director_attributes *attributes) {
  d_delete(attributes->connector_factory);
  d_delete(attributes->connectable_factory);
  d_delete(attributes->packet_factory);
  d_delete(attributes->environment);
  d_delete(attributes->ui_factory);
  d_delete(attributes->media_factory);
  return NULL;
}
d_define_class(director) {d_hook_method(director, e_flag_public, add_node),
  d_hook_method(director, e_flag_public, set_level),
  d_hook_method(director, e_flag_private, update_level),
  d_hook_method(director, e_flag_private, update_event),
  d_hook_method_override(director, e_flag_public, eventable, event),
  d_hook_method_override(director, e_flag_public, drawable, draw),
  d_hook_delete(director),
  d_hook_method_tail};
