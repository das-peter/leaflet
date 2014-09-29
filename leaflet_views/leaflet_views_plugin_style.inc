<?php

/**
 * @file
 * Extension of the Views Plugin Style for Leaflet Map
 * Adapted from the GeoField Map views module and the OpenLayers Views module.
 */

class leaflet_views_plugin_style extends views_plugin_style {
  /**
   * Store the entity type.
   * @var string
   */
  protected $entity_type;

  /**
   * Store the entity info array.
   * @var array
   */
  protected $entity_info;

  /**
   * If this view is displaying an entity, save the entity type and info.
   */
  function init(&$view, &$display, $options = NULL) {
    parent::init($view, $display, $options);

    // Search api indexes store the entity metadata in the views data array.
    if (strpos($view->base_table, 'search_api_index_') === 0) {
      $views_data = views_fetch_data();
      if (isset($views_data[$view->base_table]['table']['entity type'])) {
        $this->entity_type = $views_data[$view->base_table]['table']['entity type'];
        $this->entity_info = entity_get_info($this->entity_type);
      }
    }
    else {
      foreach (entity_get_info() as $key => $info) {
        if ($view->base_table == $info['base table']) {
          $this->entity_type = $key;
          $this->entity_info = $info;
          break;
        }
      }
    }
  }

  /**
   * Set default options.
   */
  function option_definition() {
    $options = parent::option_definition();
    $options['data_source'] = array('default' => '');
    $options['name_field'] = array('default' => '');
    $options['description_field'] = array('default' => '');
    $options['view_mode'] = array('default' => 'full');
    $options['map'] = array('default' => '');
    $options['height'] = array('default' => '400');
    $options['hide_empty'] = array('default' => '');
    $options['popup']['contains'] = array(
      'show' => array('default' => ''),
      'text' => array('default' => ''),
    );
    $options['zoom']['contains'] = array(
      'initialZoom' => array('default' => ''),
      'minZoom' => array('default' => 0),
      'maxZoom' => array('default' => 18),
    );
    $options['icon']['contains'] = array(
      'iconType' => array('default' => 'marker'),
      'iconUrl' => array('default' => ''),
      'shadowUrl' => array('default' => ''),
      'iconSize' => array(
        'contains' => array(
          'x' => array('default' => ''),
          'y' => array('default' => ''),
        )),
      'iconAnchor' => array(
        'contains' => array(
          'x' => array('default' => ''),
          'y' => array('default' => ''),
        )),
      'shadowAnchor' => array(
        'contains' => array(
          'x' => array('default' => ''),
          'y' => array('default' => ''),
        )),
      'popupAnchor' => array(
        'contains' => array(
          'x' => array('default' => ''),
          'y' => array('default' => ''),
        )),
      'html' => array('default' => ''),
      'iconImageStyle' => array('default' => ''),
      'htmlClass' => array('default' => ''),
    );
    $options['vector_display']['contains'] = array(
      'stroke_override' => array('default' => 0),
      'stroke' => array('default' => 1),
      'color' => array('default' => ''),
      'weight' => array('default' => ''),
      'opacity' => array('default' => ''),
      'dashArray' => array('default' => ''),
      'fill' => array('default' => 1),
      'fillColor' => array('default' => ''),
      'fillOpacity' => array('default' => ''),
      'clickable' => array('default' => 1),
    );
    return $options;
  }

  /**
   * Options form.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    // Get list of fields in this view & flag available geodata fields:
    $handlers = $this->display->handler->get_handlers('field');

    $fields = array();
    $fields_data = array();
    foreach ($handlers as $field_id => $handler) {
      $fields[$field_id] = $handler->ui_name();

      $field_info = NULL;
      if (!empty($handler->field_info)) {
        $field_info = $handler->field_info;
      }
      elseif ($this->is_entity_views_handler($handler)) {
        // Strip the basic field name from the entity views handler field and
        // fetch the field info for it.
        $field_name = ltrim(substr($handler->real_field, 0, strpos($handler->real_field, ':')), ':');
        $field_info = field_info_field($field_name);
      }

      if (!empty($field_info['type']) && $field_info['type'] == 'geofield') {
        $fields_data[$field_id] = $handler->ui_name();
      }
    }

    // Check whether we have a geofield we can work with:
    if (!count($fields_data)) {
      $form['error'] = array(
        '#markup' => t('Please add at least one geofield to the view'),
      );
      return;
    }

    // Map preset.
    $form['data_source'] = array(
      '#type' => 'select',
      '#title' => t('Data Source'),
      '#description' => t('Which field contains geodata?'),
      '#options' => $fields_data,
      '#default_value' => $this->options['data_source'],
      '#required' => TRUE,
    );

    // Name field.
    $form['name_field'] = array(
      '#type' => 'select',
      '#title' => t('Title Field'),
      '#description' => t('Choose the field which will appear as a title on tooltips.'),
      // '#options' => $fields,
      '#options' => array_merge(array('' => ''), $fields),
      '#default_value' => $this->options['name_field'],
    );

    $desc_options = array_merge(array(
        '' => '',
    ), $fields);

    // Add an option to render the entire entity using a view mode.
    if (!empty($this->entity_type)) {
      $desc_options += array(
        '#rendered_entity' => '<' . t('!entity entity', array('!entity' => $this->entity_type)) . '>',
      );
    }

    $form['description_field'] = array(
      '#type' => 'select',
      '#title' => t('Description Content'),
      '#description' => t('Choose the field or rendering method which will appear as a description on tooltips or popups.'),
      '#required' => FALSE,
      '#options' => $desc_options,
      '#default_value' => $this->options['description_field'],
    );

    // Taken from openlayers_views_style_data::options_form().
    // Create view mode options:
    if (!empty($this->entity_type)) {

      // Get the labels (human readable) of the view modes:
      $view_mode_options = array();
      foreach ($this->entity_info['view modes'] as $key => $view_mode) {
        $view_mode_options[$key] = $view_mode['label'];
      }

      // Output the form:
      $form['view_mode'] = array(
        '#type' => 'select',
        '#title' => t('View mode'),
        '#description' => t('View modes are ways of displaying entities.'),
        '#options' => $view_mode_options,
        '#default_value' => !empty($this->options['view_mode']) ? $this->options['view_mode'] : 'full',
        '#states' => array(
          'visible' => array(
            ':input[name="style_options[description_field]"]' => array('value' => '#rendered_entity'),
          ),
        ),
      );
    }

    // Choose a map preset:
    $map_options = array();
    foreach (leaflet_map_get_info() as $key => $map) {
      $map_options[$key] = t('@label', array('@label' => $map['label']));
    }

    $form['map'] = array(
      '#title' => t('Map'),
      '#type' => 'select',
      '#options' => $map_options,
      '#default_value' => $this->options['map'] ? $this->options['map'] : '',
      '#required' => TRUE,
    );

    $form['height'] = array(
      '#title' => t('Map height'),
      '#type' => 'textfield',
      '#field_suffix' => t('px'),
      '#size' => 4,
      '#default_value' => $this->options['height'],
      '#required' => FALSE,
    );

    $form['hide_empty'] = array(
      '#title' => t('Hide empty'),
      '#type' => 'checkbox',
      '#description' => t('Hide the Leaflet map if there are no results to display.'),
      '#default_value' => isset($this->options['hide_empty']) ? $this->options['hide_empty'] : TRUE,
    );

    $form['zoom'] = leaflet_form_elements('zoom', $this->options);
    $form['icon'] = leaflet_form_elements('icon', $this->options, array('path' => 'style_options', 'fields' => $fields));
    $form['vector_display'] = leaflet_form_elements('vector_display', $this->options, array('path' => 'style_options'));
    $token_options = array('weight' => 998, 'entity_type' => NULL);
    if (!empty($this->entity_type)) {
      $token_options['entity_type'] = $this->entity_type;
    }
    $form['tokens'] = leaflet_form_elements('tokens', $this->options, $token_options);
  }

  /**
   * Validate the options form.
   */
  function options_validate(&$form, &$form_state) {
    if (!is_numeric($form_state['values']['style_options']['height']) || $form_state['values']['style_options']['height'] < 0) {
      form_error($form['height'], t('Map height needs to be a positive number'));
    }
  }

  /**
   * Renders view.
   */
  function render() {
    if (!empty($this->view->live_preview)) {
      return t('No preview available');
    }
    $data = array();
    $map = leaflet_map_get_info($this->options['map']);
    // Fetch result entities if necessary.
    $entities = array();
    if ($this->options['description_field'] === '#rendered_entity') {
      list(, $entities) = $this->view->query->get_result_entities($this->view->result);
    }
    // Is there a geofield selected?
    if ($this->options['data_source']) {
      $this->render_fields($this->view->result);
      // Support for entity views integration.
      $geofield_handler = $this->view->display_handler->get_handler('field', $this->options['data_source']);
      if (($is_entity_views_handler = $this->is_entity_views_handler($geofield_handler))) {
        $geofield_property = EntityFieldHandlerHelper::get_selector_field_name($geofield_handler->real_field);
      }

      foreach ($this->view->result as $id => $result) {
        $geofield = $this->get_field_value($id, $this->options['data_source']);

        if (!empty($geofield)) {
          // Check if this field is handled by the entity views integration.
          if ($is_entity_views_handler) {
            // Use the entity field wrapper to fetch all the data an not just
            // a single property.
            $geofield = array($geofield_handler->wrappers[$id]->raw());
          }

          $entity = FALSE;
          // Render the entity with the selected view mode:
          if ($this->options['description_field'] === '#rendered_entity') {
            // If the entity wasn't fetched previously try our best to load it.
            if (!isset($entities[$id]) && isset($result->{$this->entity_info['entity keys']['id']})) {
              $entities[$id] = entity_load_single($this->entity_type, $result->{$this->entity_info['entity keys']['id']});
            }
            $build = entity_view($this->entity_type, array($entities[$id]), $this->options['view_mode']);
            $description = drupal_render($build);
          }
          // Normal rendering via fields:
          elseif ($this->options['description_field']) {
            $description = $this->rendered_fields[$id][$this->options['description_field']];
          }

          $points = leaflet_process_geofield($geofield);
          // Attach pop-ups if we have rendered into $description:
          if (isset($description)) {
            foreach ($points as &$point) {
              $point['popup'] = $description;
            }
          }
          // Attach also titles & entities, they might be used later on.
          if ($this->options['name_field']) {
            foreach ($points as &$point) {
              $point['label'] = $this->rendered_fields[$id][$this->options['name_field']];
              if ($entity !== FALSE) {
                $point['entity'] = $entity;
              }
            }
          }
          // Let modules modify the points data.
          drupal_alter('leaflet_views_alter_points_data', $result, $points);
          // Merge these points into the $data array for map rendering:
          $data = array_merge($data, $points);
        }
      }
      leaflet_apply_map_settings($map, $data, $this->options, $this->entity_type);
      if (empty($data) && !empty($this->options['hide_empty'])) {
        return '';
      }
      return leaflet_build_map($map, $data, $this->options['height'] . 'px');
    }
    return '';
  }

  /**
   * Checks if a field handler class is handled by the entity module.
   *
   * @param object $field_handler_instance
   *   The field handler instance to check.
   *
   * @return bool
   *   TRUE if the field is handled by the entity module views integration.
   */
  public function is_entity_views_handler($field_handler_instance) {
    $static_cache = &drupal_static(__METHOD__, array());
    $handler_class = get_class($field_handler_instance);

    if (!isset($static_cache[$handler_class])) {
      $static_cache[$handler_class] = FALSE;
      foreach (entity_views_get_field_handlers() as $field_handler) {
        if ($field_handler_instance instanceof $field_handler) {
          return $static_cache[$handler_class] = TRUE;
        }
      }
    }
    return $static_cache[$handler_class];
  }
}
