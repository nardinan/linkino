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
#include "connector_factory.obj.h"
struct s_connector_factory_attributes *p_connector_factory_alloc(struct s_object *self) {
  struct s_connector_factory_attributes *result = d_prepare(self, connector_factory);
  f_memory_new(self);                                                                 /* inherit */
  f_mutex_new(self);                                                                  /* inherit */
  f_drawable_new(self, (e_drawable_kind_single | e_drawable_kind_force_visibility));  /* inherit */
  f_eventable_new(self);                                                              /* inherit */
  return result;
}
struct s_object *f_connector_factory_new(struct s_object *self, struct s_object *drawable) {
  struct s_connector_factory_attributes *connector_factory_attributes = p_connector_factory_alloc(self);
  if ((connector_factory_attributes->drawable = d_retain(drawable))) {
    connector_factory_attributes->array_of_connectors = f_array_new(d_new(array), d_array_bucket);
  }
  return self;
}
d_define_method(connector_factory, set_drop)(struct s_object *self, t_boolean approve_drop, struct s_connectable_link *link) {
  d_using(connector_factory);
  if (approve_drop) {
    /* we need to be sure that we can approve the drop. The source and the destination have to be different and the link should not be already
     * linked to something else */
    if (((!link) || (!link->connector)) && ((!link) || (!connector_factory_attributes->source_link) ||
          (f_string_strcmp(connector_factory_attributes->source_link->unique_code, link->unique_code) != 0))) {
      if (connector_factory_attributes->source_link)
        connector_factory_attributes->destination_link = link;
      else
        connector_factory_attributes->source_link = link;
      connector_factory_attributes->approve_drop = d_true;
    } else {
      connector_factory_attributes->approve_drop = d_false;
      connector_factory_attributes->destination_link = NULL;
    }
  } else {
    connector_factory_attributes->approve_drop = d_false;
  }
  return self;
}
d_define_method(connector_factory, get_connector_with_source)(struct s_object *self, struct s_connectable_link *link) {
  d_using(connector_factory);
  struct s_object *current_connector, *result = NULL;
  if (connector_factory_attributes->array_of_connectors)
    d_array_foreach(connector_factory_attributes->array_of_connectors, current_connector) {
      struct s_connector_attributes *connector_attributes = d_cast(current_connector, connector);
      if (connector_attributes->source_link == link) {
        result = current_connector;
        break;
      }
    }
  return result;
}
d_define_method(connector_factory, get_connector_with_destination)(struct s_object *self, struct s_connectable_link *link) {
  d_using(connector_factory);
  struct s_object *current_connector, *result = NULL;
  if (connector_factory_attributes->array_of_connectors)
    d_array_foreach(connector_factory_attributes->array_of_connectors, current_connector) {
      struct s_connector_attributes *connector_attributes = d_cast(current_connector, connector);
      if (connector_attributes->destination_link == link) {
        result = current_connector;
        break;
      }
    } 
  return result;
}
d_define_method(connector_factory, is_reachable)(struct s_object *self, struct s_connectable_link *ingoing_link, const char *destination, char *wr_visited, 
    unsigned int *hops) {
  d_using(connector_factory);
  unsigned int global_hops_detected;
  struct s_connectable_link *result = NULL;
  struct s_object *connector;
  size_t entries;
  strcat(wr_visited, " ");
  strcat(wr_visited, ingoing_link->unique_code);
  d_call(connector_factory_attributes->array_of_connectors, m_array_size, &entries);
  for (size_t index = 0; index < entries; ++index)
    if ((connector = d_call(connector_factory_attributes->array_of_connectors, m_array_get, index))) {
      struct s_connector_attributes *connector_attributes = d_cast(connector, connector);
      struct s_connectable_link *outgoing_link = NULL; 
      if ((f_string_strcmp(connector_attributes->source_link->unique_code, ingoing_link->unique_code) == 0))
        outgoing_link = connector_attributes->destination_link;
      else if ((f_string_strcmp(connector_attributes->destination_link->unique_code, ingoing_link->unique_code) == 0))
        outgoing_link = connector_attributes->source_link;
      if (outgoing_link) {
        if ((f_string_strcmp(outgoing_link->unique_code, destination) == 0)) {
          /* we found it, is here, is impossible that on the same node we might find something less far from here */
          global_hops_detected = (*hops + 1);
          result = outgoing_link;
        } else if (!strstr(wr_visited, outgoing_link->unique_code)) {
          /* has not been visited yet so we can go deeper */
          unsigned int local_hops_detected = (*hops + 1);
          if (((intptr_t)d_call(self, m_connector_factory_is_reachable, outgoing_link, destination, wr_visited, &local_hops_detected)))
            /* we found it passing through this path, so, in case the global number of hops is bigger, we normalize it to this one */
            if ((!result) || (local_hops_detected < global_hops_detected)) {
              global_hops_detected = local_hops_detected;
              result = outgoing_link;
            }
        }
      }
    }
  if (result) {
    *hops = global_hops_detected;
  }
  d_cast_return(result);
}
d_define_method(connector_factory, get_connector_for)(struct s_object *self, struct s_connectable_link *ingoing_link, 
    struct s_connectable_link *previous_ingoing_link, const char *destination, unsigned int *hops) {
  char buffer_already_visited_nodes[d_string_buffer_size] = {0};
  *hops = 0;
  if (previous_ingoing_link)
    strcat(buffer_already_visited_nodes, previous_ingoing_link->unique_code);
  d_cast_return(d_call(self, m_connector_factory_is_reachable, ingoing_link, destination, buffer_already_visited_nodes, hops));
}
d_define_method(connector_factory, check_snapped)(struct s_object *self) {
  d_using(connector_factory);
  size_t entries;
  t_boolean connector_deleted = d_true;
  d_call(connector_factory_attributes->array_of_connectors, m_array_size, &entries);
  while (connector_deleted) {
    struct s_object *current_connector;
    connector_deleted = d_false;
    for (size_t index = 0; index < entries; ++index)
      if ((current_connector = d_call(connector_factory_attributes->array_of_connectors, m_array_get, index)))
        if (d_call(current_connector, m_connector_is_snapped, NULL)) {
          struct s_connector_attributes *connector_attributes = d_cast(current_connector, connector);
          /* the connector has been snapped! Damn! */
          printf("ARC between %s (link %s) and %s (link %s) SNAPPED due to an overload\n", connector_attributes->source_link->unique_code,
              connector_attributes->source_link->label, connector_attributes->destination_link->unique_code, connector_attributes->destination_link->label);
          ++(connector_factory_attributes->snapped_connectors);
          d_call(current_connector, m_connector_destroy_links, NULL);
          d_call(connector_factory_attributes->array_of_connectors, m_array_remove, index);
          d_call(connector_factory_attributes->array_of_connectors, m_array_shrink, NULL);
          connector_deleted = d_true;
          break;
        }
    if (connector_deleted)
      --entries;
  }
  return self;
}
d_define_method_override(connector_factory, event)(struct s_object *self, struct s_object *environment, SDL_Event *current_event) {
  d_using(connector_factory);
  t_boolean changed = d_false;
  if (connector_factory_attributes->drawable) {
    int mouse_x, mouse_y;
    d_call(environment, m_environment_get_mouse_normalized, "draw_camera", &mouse_x, &mouse_y);
    if (connector_factory_attributes->active_connector) {
      /* we might be trigger by two different events:
       * - left click (drop the line where we are, store it into the array of connector and removes the active connector)
       * - right click (removes the active connector)
       * Anyway, before doing anything, we need to update the destination of the active_connector using the current position of the mouse
       * in order for the line to be correctly displayed in the screen
       */
      d_call(connector_factory_attributes->active_connector, m_connector_set_destination, (double)mouse_x, (double)mouse_y,
          connector_factory_attributes->destination_link);
      if (current_event->type == SDL_MOUSEBUTTONDOWN) {
        t_boolean reset_entry = d_false;
        if ((connector_factory_attributes->approve_drop) && (connector_factory_attributes->destination_link)) {
          if (current_event->button.button == SDL_BUTTON_LEFT) {
            d_call(connector_factory_attributes->active_connector, m_connector_set_destination, 
                connector_factory_attributes->destination_link->final_position_x, connector_factory_attributes->destination_link->final_position_y, 
                connector_factory_attributes->destination_link);
            d_call(connector_factory_attributes->array_of_connectors, m_array_push, connector_factory_attributes->active_connector);
          }
          reset_entry = d_true;
        } else if (current_event->button.button == SDL_BUTTON_RIGHT)
          reset_entry = d_true;
        if (reset_entry) {
          d_delete(connector_factory_attributes->active_connector);
          connector_factory_attributes->active_connector = NULL;
          connector_factory_attributes->source_link = NULL;
          connector_factory_attributes->destination_link = NULL;
          connector_factory_attributes->approve_drop = d_false;
        }
      }
      changed = d_true;
    } else if (current_event->type == SDL_MOUSEBUTTONDOWN) {
      if ((connector_factory_attributes->approve_drop) && (connector_factory_attributes->source_link)) {
        /* if approve_drop is active and source link is set, means that the same event has been listen by a connectable and we have clicked on its linkable 
         * point. In pratice, this means that we can drop a new connector where we are and that will be linked to a specific source_link connectable_link 
         * owned by a connectable 
         */
        if (current_event->button.button == SDL_BUTTON_LEFT) {
          connector_factory_attributes->active_connector =
            f_connector_new(d_new(connector), connector_factory_attributes->drawable, connector_factory_attributes->source_link->final_position_x,
                connector_factory_attributes->source_link->final_position_y, connector_factory_attributes->source_link);
          changed = d_true;
        }
      } else if (current_event->button.button == SDL_BUTTON_RIGHT) {
        /* if we are here, means that we just have right-clicked on the screen and we don't have any active link. We might want to delete the first
         * link that is next the mouse and that is currently active
         */
        struct s_object *current_connector;
        size_t elements;
        d_call(connector_factory_attributes->array_of_connectors, m_array_size, &elements);
        for (size_t index = 0; index < elements; ++index)
          if ((current_connector = d_call(connector_factory_attributes->array_of_connectors, m_array_get, index)))
            if (d_call(current_connector, m_connector_is_over, (int)mouse_x, (int)mouse_y)) {
              d_call(current_connector, m_connector_destroy_links, NULL);
              d_call(connector_factory_attributes->array_of_connectors, m_array_remove, index);
              d_call(connector_factory_attributes->array_of_connectors, m_array_shrink, NULL);
              break;
            }
      }
    }
  }
  d_cast_return(((changed) ? e_eventable_status_captured : e_eventable_status_ignored));
}
d_define_method_override(connector_factory, draw)(struct s_object *self, struct s_object *environment) {
  d_using(connector_factory);
  struct s_environment_attributes *environment_attributes = d_cast(environment, environment);
  struct s_camera_attributes *camera_attributes = d_cast(environment_attributes->current_camera, camera);
  if (connector_factory_attributes->drawable) {
    struct s_object *current_connector;
    d_call(self, m_connector_factory_check_snapped, NULL);
    if (connector_factory_attributes->active_connector) {
      if ((d_call(connector_factory_attributes->active_connector, m_drawable_normalize_scale, camera_attributes->scene_reference_w,
              camera_attributes->scene_reference_h, camera_attributes->scene_offset_x, camera_attributes->scene_offset_y, camera_attributes->scene_center_x,
              camera_attributes->scene_center_y, camera_attributes->screen_w, camera_attributes->screen_h, camera_attributes->scene_zoom)))
        while (((intptr_t)d_call(connector_factory_attributes->active_connector, m_drawable_draw, environment)) == d_drawable_return_continue);
    }
    d_array_foreach(connector_factory_attributes->array_of_connectors, current_connector) {
      if ((d_call(current_connector, m_drawable_normalize_scale, camera_attributes->scene_reference_w, camera_attributes->scene_reference_h,
              camera_attributes->scene_offset_x, camera_attributes->scene_offset_y, camera_attributes->scene_center_x, camera_attributes->scene_center_y,
              camera_attributes->screen_w, camera_attributes->screen_h, camera_attributes->scene_zoom)))
        while (((intptr_t)d_call(current_connector, m_drawable_draw, environment)) == d_drawable_return_continue);
    }
  }
  d_cast_return(d_drawable_return_last);
}
d_declare_method(connector_factory, delete)(struct s_object *self, struct s_connector_factory_attributes *attributes) {
  if (attributes->array_of_connectors)
    d_delete(attributes->array_of_connectors);
  if (attributes->drawable)
    d_delete(attributes->drawable);
  return NULL;
}
d_define_class(connector_factory) {d_hook_method(connector_factory, e_flag_public, set_drop),
  d_hook_method(connector_factory, e_flag_public, get_connector_with_source),
  d_hook_method(connector_factory, e_flag_public, get_connector_with_destination),
  d_hook_method(connector_factory, e_flag_private, is_reachable),
  d_hook_method(connector_factory, e_flag_public, get_connector_for),
  d_hook_method(connector_factory, e_flag_public, check_snapped),
  d_hook_method_override(connector_factory, e_flag_public, eventable, event),
  d_hook_method_override(connector_factory, e_flag_public, drawable, draw),
  d_hook_delete(connector_factory),
  d_hook_method_tail};
