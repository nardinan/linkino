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
#include "connectable_factory.obj.h"
#include "connector.obj.h"
#include "packet.obj.h"
void p_connectable_factory_click_create(struct s_object *self, void **parameters, size_t entries) {
  struct s_object *connectable_factory = (struct s_object *)parameters[0];
  struct s_connectable_factory_template *current_template = (struct s_connectable_factory_template *)parameters[1];
  struct s_object *ui_button = (struct s_object *)parameters[2];
  d_call(connectable_factory, m_connectable_factory_click_received_create, current_template);
  d_call(ui_button, m_uiable_mode, e_uiable_mode_idle);
}
void p_connectable_factory_click_sell(struct s_object *self, void **parameters, size_t entries) {
  struct s_object *connectable_factory = (struct s_object *)parameters[0];
  struct s_object *ui_button = (struct s_object *)parameters[1];
  d_call(connectable_factory, m_connectable_factory_click_received_sell, NULL);
  /* due to the fact that the interface will be removed immediately, the system needs to manually 
   * reset the status of the button */
  d_call(ui_button, m_uiable_mode, e_uiable_mode_idle);
}
void p_connectable_factory_click_cancel(struct s_object *self, void **parameters, size_t entries) {
  struct s_object *connectable_factory = (struct s_object *)parameters[0];
  struct s_object *ui_button = (struct s_object *)parameters[1];
  d_call(connectable_factory, m_connectable_factory_click_received_cancel, NULL);
  /* due to the fact that the interface will be removed immediately, the system needs to manually 
   * reset the status of the button */
  d_call(ui_button, m_uiable_mode, e_uiable_mode_idle);
}
struct s_connectable_factory_attributes *p_connectable_factory_alloc(struct s_object *self) {
  struct s_connectable_factory_attributes *result = d_prepare(self, connectable_factory);
  f_memory_new(self);                                                                 /* inherit */
  f_mutex_new(self);                                                                  /* inherit */
  f_drawable_new(self, (e_drawable_kind_single | e_drawable_kind_force_visibility));  /* inherit */
  f_eventable_new(self);                                                              /* inherit */
  return result;
}
struct s_object *f_connectable_factory_new(struct s_object *self, struct s_object *ui_factory, struct s_object *environment) {
  struct s_connectable_factory_attributes *connectable_factory_attributes = p_connectable_factory_alloc(self);
  struct s_uiable_container *container;
  connectable_factory_attributes->ui_factory = d_retain(ui_factory);
  connectable_factory_attributes->environment = d_retain(environment);
  memset(&(connectable_factory_attributes->list_templates), 0, sizeof(struct s_list));
  d_assert(connectable_factory_attributes->array_connectable_instances = f_array_new(d_new(array), d_array_bucket));
  if ((container = d_call(connectable_factory_attributes->ui_factory, m_ui_factory_get_component, NULL, "confirmation_sell"))) {
    struct s_uiable_container *button_ok, *button_cancel;
    struct s_eventable_attributes *eventable_attributes = d_cast(container->uiable, eventable);
    connectable_factory_attributes->ui_sell_confirmation = container->uiable;
    d_assert(button_ok = d_call(connectable_factory_attributes->ui_factory, m_ui_factory_get_component, container, "button_ok"));
    d_call(button_ok->uiable, m_emitter_embed_parameter, "clicked_left", self);
    d_call(button_ok->uiable, m_emitter_embed_parameter, "clicked_left", button_ok->uiable);
    d_call(button_ok->uiable, m_emitter_embed_function, "clicked_left", p_connectable_factory_click_sell);
    d_assert(button_cancel = d_call(connectable_factory_attributes->ui_factory, m_ui_factory_get_component, container, "button_cancel"));
    d_call(button_cancel->uiable, m_emitter_embed_parameter, "clicked_left", self);
    d_call(button_cancel->uiable, m_emitter_embed_parameter, "clicked_left", button_cancel->uiable);
    d_call(button_cancel->uiable, m_emitter_embed_function, "clicked_left", p_connectable_factory_click_cancel);
    eventable_attributes->ignore_event_if_consumed = d_false;
  } else
    d_die(d_error_malloc);
  return self;
}
d_define_method(connectable_factory, add_connectable_template)(struct s_object *self, struct s_object *stream, const char *title, const char *description,
    double *offsets_x, double *offsets_y, size_t connections, unsigned int price, t_boolean generate_traffic, t_boolean filter_spam) {
  d_using(connectable_factory);
  if (connections <= d_connectable_factory_connections) {
    struct s_connectable_factory_template *current_template;
    if ((current_template = (struct s_connectable_factory_template *)d_malloc(sizeof(struct s_connectable_factory_template)))) {
      current_template->stream = d_retain(stream);
      strncpy(current_template->title, title, d_string_buffer_size);
      strncpy(current_template->description, description, d_string_buffer_size);
      memcpy(current_template->offsets_x, offsets_x, (sizeof(double) * connections));
      memcpy(current_template->offsets_y, offsets_y, (sizeof(double) * connections));
      current_template->price = price;
      current_template->generate_traffic = generate_traffic;
      current_template->filter_spam = filter_spam;
      current_template->connections = connections;
      if ((current_template->uiable_button =
            d_call(connectable_factory_attributes->ui_factory, m_ui_factory_new_button, d_ui_factory_default_font_id, d_ui_factory_default_font_style,
              current_template->title))) {
        d_call(current_template->uiable_button, m_emitter_embed_parameter, "clicked_left", self);
        d_call(current_template->uiable_button, m_emitter_embed_parameter, "clicked_left", current_template);
        d_call(current_template->uiable_button, m_emitter_embed_parameter, "clicked_left", current_template->uiable_button);
        d_call(current_template->uiable_button, m_emitter_embed_function, "clicked_left", p_connectable_factory_click_create);
        d_call(current_template->uiable_button, m_drawable_set_dimension, d_connectable_factory_button_width, d_connectable_factory_button_height);
        if ((current_template->drawable_icon = f_bitmap_new(d_new(bitmap), current_template->stream, connectable_factory_attributes->environment))) {
          f_list_append(&(connectable_factory_attributes->list_templates), (struct s_list_node *)current_template, e_list_insert_tail);
        } else
          d_die(d_error_malloc);
      } else
        d_die(d_error_malloc);
    } else
      d_die(d_error_malloc);
  }
  return self;
}
d_define_method(connectable_factory, set_connector_selected)(struct s_object *self, t_boolean selected) {
  d_using(connectable_factory);
  connectable_factory_attributes->connector_selected = selected;
  return self;
}
d_define_method(connectable_factory, set_credit)(struct s_object *self, unsigned int credit) {
  d_using(connectable_factory);
  connectable_factory_attributes->current_credit = credit;
  return self;
}
d_define_method(connectable_factory, get_selected_node)(struct s_object *self) {
  d_using(connectable_factory);
  struct s_connectable_link *result = connectable_factory_attributes->active_node;
  /* the routine takes the latest selected node, then it resets the pointer to NULL in order to be ready for the next release */
  connectable_factory_attributes->active_node = NULL;
  d_cast_return(result);
}
d_define_method(connectable_factory, is_traffic_generation_required)(struct s_object *self) {
  d_using(connectable_factory);
  struct s_object *current_connectable;
  struct s_connectable_link *result = NULL;
  d_array_foreach(connectable_factory_attributes->array_connectable_instances, current_connectable) {
    if ((result = (struct s_connectable_link *)d_call(current_connectable, m_connectable_is_traffic_generation_required, NULL))) {
      break;
    }
  }
  d_cast_return(result);
}
d_define_method(connectable_factory, click_received_create)(struct s_object *self, struct s_connectable_factory_template *template) {
  d_using(connectable_factory);
  if (!connectable_factory_attributes->active_template)
    if (template->price <= connectable_factory_attributes->current_credit) {
      connectable_factory_attributes->current_credit -= template->price;
      connectable_factory_attributes->active_template = template;
    }
  return self;
}
d_define_method(connectable_factory, click_received_sell)(struct s_object *self) {
  d_using(connectable_factory);
  size_t elements;
  struct s_object *current_connectable;
  d_call(connectable_factory_attributes->array_connectable_instances, m_array_size, &elements);
  for (size_t index = 0; index < elements; ++index)
    if ((current_connectable = d_call(connectable_factory_attributes->array_connectable_instances, m_array_get, index)))
      if (current_connectable == connectable_factory_attributes->delete_connectable) {
        struct s_connectable_attributes *connectable_attributes = d_cast(current_connectable, connectable);
        struct s_connectable_link *current_link;
        struct s_object *current_packet;
        /* we unlink all the connectors that are currently linked to this specific node */
        d_foreach(&(connectable_attributes->list_connection_nodes), current_link, struct s_connectable_link) {
          for (size_t index = 0; index < d_connectable_max_packets; ++index) 
            if ((current_packet = current_link->traveling_packets[index])) {
              current_link->traveling_packets[index] = NULL;
              d_call(current_packet, m_packet_destroy_links, NULL);
              d_delete(current_packet);
            }
          if (current_link->connector) {
            d_call(current_link->connector, m_connector_destroy_links, NULL);
            current_link->connector = NULL;
          }
        }
        /* we give back to our bank account half of the price of the tool */
        connectable_factory_attributes->current_credit += (connectable_attributes->price / 2.0);
        d_call(connectable_factory_attributes->array_connectable_instances, m_array_remove, index);
        d_call(connectable_factory_attributes->array_connectable_instances, m_array_shrink, NULL);
        break;
      }
  connectable_factory_attributes->delete_connectable = NULL;
  d_call(connectable_factory_attributes->environment, m_environment_del_drawable, connectable_factory_attributes->ui_sell_confirmation, 
      (d_ui_factory_default_level + 1), e_environment_surface_primary);
  connectable_factory_attributes->ui_sell_confirmation_visible = d_false;
  return self;
}
d_define_method(connectable_factory, click_received_cancel)(struct s_object *self) {
  d_using(connectable_factory);
  connectable_factory_attributes->delete_connectable = NULL;
  d_call(connectable_factory_attributes->environment, m_environment_del_drawable, connectable_factory_attributes->ui_sell_confirmation, 
      (d_ui_factory_default_level + 1), e_environment_surface_primary);
  connectable_factory_attributes->ui_sell_confirmation_visible = d_false;
  return self;
}
d_define_method_override(connectable_factory, event)(struct s_object *self, struct s_object *environment, SDL_Event *current_event) {
  d_using(connectable_factory);
  struct s_object *current_connectable;
  t_boolean changed = d_false;
  /* OK we probably have different things to do, but some of these things cannot be done while a connector is active. I will list here the things 
   * and I will make them D as doable while a connector is active or N as not doable while connector is active.
   * - (D) if a connectable is active, we need to make it following the mouse and listen in order to drop it where click is received
   * - (D) if a template is active, we need to make it following the mouse and listen in order to drop it where click is received 
   * - (D) if a link of a connectable already present is clicked, we need to select it as active_node (however, this specific action should return 
   *       a failed event in order to forward to the lower components (packet and connector) the event.
   * - (N) we need to check if a button is clicked to create a new template
   * - (N) we need to check if we are clicking over an already dropped connectable and move it around (active_connectable)
   *
   * Remember that active connectable has always the priority over active_template.
   */
  if (connectable_factory_attributes->delete_connectable) { 
    /* we have an object that is supposed to be deleted. For the time being there is nothing to do, we can just ignore. However we should mark the 
     * event as "changed"  because we don't want anybody else to do it
     */
    if (!connectable_factory_attributes->ui_sell_confirmation_visible) {
      d_call(connectable_factory_attributes->environment, m_environment_add_drawable, connectable_factory_attributes->ui_sell_confirmation,
          (d_ui_factory_default_level + 1), e_environment_surface_primary);
      connectable_factory_attributes->ui_sell_confirmation_visible = d_true;
    }
    changed = d_true;
  } else if (connectable_factory_attributes->active_connectable) {
    if ((current_event->type == SDL_MOUSEBUTTONDOWN) && (current_event->button.button == SDL_BUTTON_LEFT))
      connectable_factory_attributes->active_connectable = NULL;
    changed = d_true;
  } else if (connectable_factory_attributes->active_template) {
    if ((current_event->type == SDL_MOUSEBUTTONDOWN) && (current_event->button.button == SDL_BUTTON_LEFT)) {
      char buffer[d_string_buffer_size];
      /* we generate a random string that will be used to identify univocally the instance */
      /* we drop the active template and we create a new connectable that we push into the array */
      struct s_object *connectable =
        f_connectable_new(d_new(connectable), connectable_factory_attributes->active_template->stream, 
            connectable_factory_attributes->environment, connectable_factory_attributes->ui_factory, 
            ((connectable_factory_attributes->active_template->generate_traffic)?d_true:d_false), 
            connectable_factory_attributes->active_template->filter_spam);
      d_call(connectable, m_connectable_set_generate_traffic, connectable_factory_attributes->active_template->generate_traffic);
      d_call(connectable, m_drawable_set_position, connectable_factory_attributes->active_template->position_x,
          connectable_factory_attributes->active_template->position_y);
      d_call(connectable, m_connectable_set_price, connectable_factory_attributes->active_template->price);
      for (size_t index_offset = 0; index_offset < connectable_factory_attributes->active_template->connections; ++index_offset) {
        snprintf(buffer, d_string_buffer_size, "%c", (char)(((char)'A') + index_offset));
        d_call(connectable, m_connectable_add_connection_point, connectable_factory_attributes->active_template->offsets_x[index_offset],
            connectable_factory_attributes->active_template->offsets_y[index_offset], buffer);
      }
      d_call(connectable_factory_attributes->array_connectable_instances, m_array_push, connectable);
      connectable_factory_attributes->active_template = NULL;
      d_delete(connectable);
    }
    changed = d_true;
  } else {
    d_array_foreach(connectable_factory_attributes->array_connectable_instances, current_connectable)
      if (((intptr_t)d_call(current_connectable, m_eventable_event, environment, current_event)) == e_eventable_status_captured) {
        struct s_connectable_link *connection_node;
        if ((connection_node = d_call(current_connectable, m_connectable_get_selected_node, NULL))) {
          connectable_factory_attributes->active_node = connection_node;
          break;
        }
      }
    /* if a node has been activated, we can ignore the rest. Moreover, since we have to forward the event to the lower event components, we 
     * need to be sure that the 'changed' flag is false
     */
    if (!connectable_factory_attributes->active_node)
      if ((!connectable_factory_attributes->connector_selected) && (!changed)) {
        struct s_connectable_factory_template *current_template;
        d_foreach(&(connectable_factory_attributes->list_templates), current_template, struct s_connectable_factory_template)
          d_call_owner(current_template->uiable_button, uiable, m_eventable_event, environment, current_event);
        /* we need to consider what happens if the button is clicked (so we have an active template) */
        if (!connectable_factory_attributes->active_template) {
          if (current_event->type == SDL_MOUSEBUTTONDOWN) {
            int mouse_x, mouse_y;
            d_call(environment, m_environment_get_mouse_normalized, "draw_camera", &mouse_x, &mouse_y);
            d_array_foreach(connectable_factory_attributes->array_connectable_instances, current_connectable)
              if (d_call(current_connectable, m_connectable_is_over, mouse_x, mouse_y)) {
                if (current_event->button.button == SDL_BUTTON_LEFT) {
                  connectable_factory_attributes->active_connectable = current_connectable;
                  changed = d_true;
                  break;
                } else if (current_event->button.button == SDL_BUTTON_RIGHT) {
                  connectable_factory_attributes->delete_connectable = current_connectable;
                  changed = d_true;
                  break;
                }
              }
          }
        } else
          changed = d_true;
      }
  }
  d_cast_return(((changed) ? e_eventable_status_captured : e_eventable_status_ignored));
}
d_define_method_override(connectable_factory, draw)(struct s_object *self, struct s_object *environment) {
  d_using(connectable_factory);
  struct s_environment_attributes *environment_attributes = d_cast(environment, environment);
  struct s_camera_attributes *camera_attributes = d_cast(environment_attributes->current_camera, camera);
  struct s_connectable_factory_template *current_template;
  struct s_object *current_connectable;
  double position_y = 0, position_x = (camera_attributes->scene_reference_w - d_connectable_factory_button_width - 1), image_width, image_height;
  int mouse_x, mouse_y;
  d_call(environment, m_environment_get_mouse_normalized, "draw_camera", &mouse_x, &mouse_y);
  if (connectable_factory_attributes->active_connectable) {
    d_call(connectable_factory_attributes->active_connectable, m_drawable_get_dimension, &image_width, &image_height);
    d_call(connectable_factory_attributes->active_connectable, m_drawable_set_position, (double)(mouse_x - (image_width / 2.0)), 
        (double)(mouse_y - (image_width / 2.0)));
  }
  d_foreach(&(connectable_factory_attributes->list_templates), current_template, struct s_connectable_factory_template) {
    d_call(current_template->uiable_button, m_drawable_set_position, position_x, position_y);
    if ((d_call(current_template->uiable_button, m_drawable_normalize_scale, camera_attributes->scene_reference_w, camera_attributes->scene_reference_h,
            camera_attributes->scene_offset_x, camera_attributes->scene_offset_y, camera_attributes->scene_center_x, camera_attributes->scene_center_y,
            camera_attributes->screen_w, camera_attributes->screen_h, camera_attributes->scene_zoom)))
      while (((intptr_t)d_call(current_template->uiable_button, m_drawable_draw, environment)) == d_drawable_return_continue);
    position_y += (d_connectable_factory_button_height + 1);
  }
  d_array_foreach(connectable_factory_attributes->array_connectable_instances, current_connectable) {
    if (connectable_factory_attributes->active_connectable == current_connectable)
      d_call(current_connectable, m_drawable_set_maskA, (unsigned int)d_connectable_factory_alpha);
    else
      d_call(current_connectable, m_drawable_set_maskA, (unsigned int)255);
    if ((d_call(current_connectable, m_drawable_normalize_scale, camera_attributes->scene_reference_w, camera_attributes->scene_reference_h,
            camera_attributes->scene_offset_x, camera_attributes->scene_offset_y, camera_attributes->scene_center_x, camera_attributes->scene_center_y,
            camera_attributes->screen_w, camera_attributes->screen_h, camera_attributes->scene_zoom)))
      while (((intptr_t)d_call(current_connectable, m_drawable_draw, environment)) == d_drawable_return_continue);
  }
  /* we have something selected, so we need to keep the icon floating around following the mouse */
  if (connectable_factory_attributes->active_template) {
    d_call(connectable_factory_attributes->active_template->drawable_icon, m_drawable_get_dimension, &image_width, &image_height);
    connectable_factory_attributes->active_template->position_x = mouse_x - (image_width / 2.0);
    connectable_factory_attributes->active_template->position_y = mouse_y - (image_height / 2.0);
    d_call(connectable_factory_attributes->active_template->drawable_icon, m_drawable_set_position, connectable_factory_attributes->active_template->position_x,
        connectable_factory_attributes->active_template->position_y);
    d_call(connectable_factory_attributes->active_template->drawable_icon, m_drawable_set_maskA, d_connectable_factory_alpha);
    if ((d_call(connectable_factory_attributes->active_template->drawable_icon, m_drawable_normalize_scale, camera_attributes->scene_reference_w,
            camera_attributes->scene_reference_h, camera_attributes->scene_offset_x, camera_attributes->scene_offset_y, camera_attributes->scene_center_x,
            camera_attributes->scene_center_y, camera_attributes->screen_w, camera_attributes->screen_h, camera_attributes->scene_zoom)))
      while (((intptr_t)d_call(connectable_factory_attributes->active_template->drawable_icon, m_drawable_draw, environment)) == d_drawable_return_continue);
  }
  d_cast_return(d_drawable_return_last);
}
d_define_method(connectable_factory, reset)(struct s_object *self) {
  d_using(connectable_factory);
  struct s_object *current_connectable;
  d_array_foreach(connectable_factory_attributes->array_connectable_instances, current_connectable) {
    struct s_connectable_attributes *connectable_attributes = d_cast(current_connectable, connectable);
    struct s_connectable_link *current_link;
    struct s_object *current_packet;
    /* we unlink all the connectors that are currently linked to this specific node */
    d_foreach(&(connectable_attributes->list_connection_nodes), current_link, struct s_connectable_link) {
      for (size_t index = 0; index < d_connectable_max_packets; ++index) 
        if ((current_packet = current_link->traveling_packets[index])) {
          current_link->traveling_packets[index] = NULL;
          d_call(current_packet, m_packet_destroy_links, NULL);
          d_delete(current_packet);
        }
      if (current_link->connector) {
        d_call(current_link->connector, m_connector_destroy_links, NULL);
        current_link->connector = NULL;
      }
    }
  }
  d_call(connectable_factory_attributes->array_connectable_instances, m_array_clear, NULL);
  if (connectable_factory_attributes->ui_sell_confirmation_visible) {
    d_call(connectable_factory_attributes->environment, m_environment_del_drawable, connectable_factory_attributes->ui_sell_confirmation, 
        (d_ui_factory_default_level + 1), e_environment_surface_primary);
    connectable_factory_attributes->ui_sell_confirmation_visible = d_false;
  }
  connectable_factory_attributes->active_connectable = NULL;
  connectable_factory_attributes->delete_connectable = NULL;
  connectable_factory_attributes->active_node = NULL;
  return self;
}
d_define_method(connectable_factory, delete)(struct s_object *self, struct s_connectable_factory_attributes *attributes) {
  struct s_connectable_factory_template *current_template;
  while ((current_template = (struct s_connectable_factory_template *)attributes->list_templates.head)) {
    d_delete(current_template->uiable_button);
    d_delete(current_template->drawable_icon);
    d_free(current_template);
    f_list_delete(&(attributes->list_templates), (struct s_list_node *)attributes->list_templates.head);
  }
  return NULL;
}
d_define_class(connectable_factory) {d_hook_method(connectable_factory, e_flag_public, add_connectable_template),
  d_hook_method(connectable_factory, e_flag_public, set_connector_selected),
  d_hook_method(connectable_factory, e_flag_public, set_credit),
  d_hook_method(connectable_factory, e_flag_public, get_selected_node),
  d_hook_method(connectable_factory, e_flag_public, is_traffic_generation_required),
  d_hook_method(connectable_factory, e_flag_public, click_received_create),
  d_hook_method(connectable_factory, e_flag_public, click_received_sell),
  d_hook_method(connectable_factory, e_flag_public, click_received_cancel),
  d_hook_method_override(connectable_factory, e_flag_public, eventable, event),
  d_hook_method_override(connectable_factory, e_flag_public, drawable, draw),
  d_hook_method(connectable_factory, e_flag_public, reset),
  d_hook_delete(connectable_factory),
  d_hook_method_tail
};
