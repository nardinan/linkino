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
#include "packet_factory.obj.h"
struct s_packet_factory_attributes *p_packet_factory_alloc(struct s_object *self) {
  struct s_packet_factory_attributes *result = d_prepare(self, packet_factory);
  f_memory_new(self);                                                                 /* inherit */
  f_mutex_new(self);                                                                  /* inherit */
  f_drawable_new(self, (e_drawable_kind_single | e_drawable_kind_force_visibility));  /* inherit */
  f_eventable_new(self);                                                              /* inherit */
  return result;
}
struct s_object *f_packet_factory_new(struct s_object *self, struct s_object *ui_factory, struct s_object *media_factory, 
    struct s_object *connectable_factory, struct s_object *connector_factory) {
  struct s_packet_factory_attributes *packet_factory_attributes = p_packet_factory_alloc(self);
  packet_factory_attributes->ui_factory = d_retain(ui_factory);
  packet_factory_attributes->media_factory = d_retain(media_factory);
  packet_factory_attributes->connectable_factory = d_retain(connectable_factory);
  packet_factory_attributes->connector_factory = d_retain(connector_factory);
  d_assert(packet_factory_attributes->array_packets_traveling = f_array_new(d_new(array), d_array_bucket));
  d_assert(packet_factory_attributes->array_packets_arrived = f_array_new(d_new(array), d_array_bucket));
  memset(&(packet_factory_attributes->statistics_generated), 0, sizeof(struct s_list));
  return self;
}
d_define_method(packet_factory, create_packet)(struct s_object *self, struct s_object *environment, const char *unique_code_source,
    const char *unique_code_destination, struct s_connectable_link *ingoing_connectable_link) {
  d_using(packet_factory);
  struct s_object *connector;
  if (((connector = d_call(packet_factory_attributes->connector_factory, m_connector_factory_get_connector_with_source, ingoing_connectable_link))) ||
      ((connector = d_call(packet_factory_attributes->connector_factory, m_connector_factory_get_connector_with_destination, ingoing_connectable_link)))) {
    enum e_media_factory_media_types type;
    struct s_object *drawable_icon, *drawable_background;
    /* first of all we need to check if the statistics of the connector has been already prepared */
    if (((drawable_icon = d_call(packet_factory_attributes->media_factory, m_media_factory_get_media, "mail_icon", &type))) &&
        ((drawable_background = d_call(packet_factory_attributes->media_factory, m_media_factory_get_media, "mail_background", &type)))) {
      struct s_object *link;
      if ((link = f_packet_new(d_new(packet), packet_factory_attributes->ui_factory, drawable_icon, drawable_background, 
              "<EMPTY BODY />", (d_packet_spam)))) {
        struct s_connector_attributes *connector_attributes = d_cast(connector, connector);
        struct s_connectable_link *outgoing_connectable_link = ((connector_attributes->source_link == ingoing_connectable_link)?
            connector_attributes->destination_link:connector_attributes->source_link);
        d_call(link, m_packet_set_traveling, connector, ingoing_connectable_link, outgoing_connectable_link, unique_code_source, unique_code_destination);
        d_call(packet_factory_attributes->array_packets_traveling, m_array_push, link);
      }
    }
  }
  return self;
}
d_define_method(packet_factory, forward_packet)(struct s_object *self, struct s_object *environment, struct s_object *packet) {
  d_using(packet_factory);
  struct s_object *result = NULL;
  if ((!(intptr_t)d_call(packet, m_packet_is_arrived_to_its_destination, NULL)) && ((intptr_t)d_call(packet, m_packet_is_arrived_to_its_hop, NULL))) {
    struct s_packet_attributes *packet_attributes = d_cast(packet, packet);
    /* OK, the packet is arrived to its next HOP but, what does this mean? It means that final destination is reached or that we have just to 
     * forward it to the next hop? */
    if ((f_string_strcmp(packet_attributes->unique_final_destination, packet_attributes->outgoing_connectable_link->unique_code) != 0)) {
      struct s_connectable_attributes *connectable_attributes = d_cast(packet_attributes->outgoing_connectable_link->connectable, connectable);
      struct s_connectable_link *current_connectable_link, *next_connectable_link, *selected_ingoing_connectable_link = NULL,
                                *selected_outgoing_connectable_link = NULL;
      unsigned int current_hops, selected_hops;
      d_foreach(&(connectable_attributes->list_connection_nodes), current_connectable_link, struct s_connectable_link)
        if ((next_connectable_link = (struct s_connectable_link *)d_call(packet_factory_attributes->connector_factory,
                m_connector_factory_get_connector_for, current_connectable_link, packet_attributes->unique_final_destination, &current_hops)))
          if ((!selected_ingoing_connectable_link) || (!selected_outgoing_connectable_link) || (current_hops < selected_hops)) {
            struct s_connector_attributes *connector_attributes = d_cast(next_connectable_link->connector, connector);
            selected_outgoing_connectable_link = next_connectable_link;
            selected_ingoing_connectable_link = ((connector_attributes->source_link == selected_outgoing_connectable_link)?
                connector_attributes->destination_link:connector_attributes->source_link);
            selected_hops = current_hops;
          }
      if ((selected_ingoing_connectable_link) && (selected_outgoing_connectable_link) && 
          (selected_ingoing_connectable_link->connector == selected_outgoing_connectable_link->connector))
        d_call(packet, m_packet_set_traveling_next_hop, selected_ingoing_connectable_link->connector, selected_ingoing_connectable_link, 
            selected_outgoing_connectable_link);
    } else
      d_call(packet, m_packet_set_traveling_complete, NULL);
  }
  return result;
}
d_define_method_override(packet_factory, event)(struct s_object *self, struct s_object *environment, SDL_Event *current_event) {
  t_boolean changed = d_false;
  d_cast_return(((changed) ? e_eventable_status_captured : e_eventable_status_ignored));
}
d_define_method_override(packet_factory, draw)(struct s_object *self, struct s_object *environment) {
  d_using(packet_factory);
  struct s_environment_attributes *environment_attributes = d_cast(environment, environment);
  struct s_camera_attributes *camera_attributes = d_cast(environment_attributes->current_camera, camera);
  struct s_object *current_packet;
  struct s_connectable_link *source_connectable_link;
  d_array_foreach(packet_factory_attributes->array_packets_traveling, current_packet)
    if (!((intptr_t)d_call(current_packet, m_packet_is_arrived_to_its_destination, NULL)))
      if (!((intptr_t)d_call(current_packet, m_packet_is_arrived_to_its_hop, NULL))) {
        d_call(current_packet, m_packet_move_by, d_packet_factory_steps);
        if ((d_call(current_packet, m_drawable_normalize_scale, camera_attributes->scene_reference_w, camera_attributes->scene_reference_h,
                camera_attributes->scene_offset_x, camera_attributes->scene_offset_y, camera_attributes->scene_center_x, camera_attributes->scene_center_y,
                camera_attributes->screen_w, camera_attributes->screen_h, camera_attributes->scene_zoom)))
          while (((intptr_t)d_call(current_packet, m_drawable_draw, environment)) == d_drawable_return_continue);
      } else
        d_call(self, m_packet_factory_forward_packet, environment, current_packet);
  while ((source_connectable_link = (struct s_connectable_link *)d_call(packet_factory_attributes->connectable_factory, 
          m_connectable_factory_is_traffic_generation_required, NULL))) {
    struct s_connectable_factory_attributes *connectable_factory_attributes = d_cast(packet_factory_attributes->connectable_factory, connectable_factory);
    struct s_object *destination_connectable = NULL;
    /* first of all, we need to select a destination for our message. We'll start by generating a random number an iterating through all the objects 
     * that can actually generate traffic present in the list of connectable. We leave the loop in two conditions:
     * - we got a traffic generator that is not the source node
     * - we are not make any progress after a which means that our node is the only one that can generate traffic in the environment
     */
    int iterations = (rand() % d_packet_factory_statistics_maximum_index);
    while (iterations > 0) {
      struct s_object *current_connectable;
      int initial_iterations = iterations; /* we keep track of the iterations */
      d_array_foreach(connectable_factory_attributes->array_connectable_instances, current_connectable) {
        struct s_connectable_attributes *connectable_attributes = d_cast(current_connectable, connectable);
        if ((connectable_attributes->generate_traffic) && (f_string_strcmp(connectable_attributes->unique_code, source_connectable_link->unique_code) != 0))
          if ((--iterations) == 0) {
            destination_connectable = current_connectable;
            break;
          }
      }
      if (initial_iterations == iterations) /* there is no other node that might be used as a target in the whole system, so no reason to go ahead */
        break;
    }
    if (destination_connectable) {
      struct s_connectable_attributes *connectable_attributes = d_cast(destination_connectable, connectable);
      struct s_connectable_link *next_step_link, *current_connectable_link;
      unsigned int hops;
      d_foreach(&(connectable_attributes->list_connection_nodes), current_connectable_link, struct s_connectable_link)
        if ((next_step_link = (struct s_connectable_link *)d_call(packet_factory_attributes->connector_factory, m_connector_factory_get_connector_for, 
                source_connectable_link, connectable_attributes->unique_code, &hops)))
          /* yeah, this is reachable and 'next_step_link' contains the next node we have to reach to join the final target */
          break;
      if (next_step_link)
        d_call(self, m_packet_factory_create_packet, environment, source_connectable_link->unique_code, connectable_attributes->unique_code,
            source_connectable_link);
    }
  }
  d_cast_return(d_drawable_return_last);
}
d_define_method(packet_factory, delete)(struct s_object *self, struct s_packet_factory_attributes *attributes) {
  struct s_packet_factory_statistics *current_statistics;
  d_delete(attributes->ui_factory);
  d_delete(attributes->media_factory);
  d_delete(attributes->connectable_factory);
  d_delete(attributes->connector_factory);
  d_delete(attributes->array_packets_traveling);
  d_delete(attributes->array_packets_arrived);
  while ((current_statistics = (struct s_packet_factory_statistics *)(attributes->statistics_generated.head))) {
    f_list_delete(&(attributes->statistics_generated), (struct s_list_node *)current_statistics);
    d_free(current_statistics);
  }
  return NULL;
}
d_define_class(packet_factory) {d_hook_method(packet_factory, e_flag_public, create_packet),
  d_hook_method(packet_factory, e_flag_public, forward_packet),
  d_hook_method_override(packet_factory, e_flag_public, eventable, event),
  d_hook_method_override(packet_factory, e_flag_public, drawable, draw),
  d_hook_delete(packet_factory)};
