<?php

/**
 * Implementation of template preprocess for the view.
 */
function template_preprocess_views_bootstrap_accordion_plugin_style(&$vars) {
  _views_bootstrap_include_bootstrap_assets();

  $view = &$vars['view'];
  $title_field = $vars['options']['title_field'];

  $vars['classes_array'][] = 'panel-group';

  // Get titles.
  if (isset($view->field[$title_field])) {
    foreach (array_keys($vars['rows']) as $key) {
      $vars['titles'][$key] = $view->style_plugin->get_field($key, $title_field);
    }
  }
}

/**
 * Implementation of template preprocess for the view.
 */
function template_preprocess_views_bootstrap_carousel_plugin_style(&$vars) {
  _views_bootstrap_include_bootstrap_assets();

  backdrop_add_js(array(
    'viewsBootstrap' => array(
      'carousel' => array(
        $vars['id'] => array(
          'id' => $vars['id'],
          'name' => $vars['view']->name,
          'attributes' => array(
            'interval' => $vars['options']['interval'] ? (int)$vars['options']['interval'] : FALSE,
            'pause' => $vars['options']['pause'] ? 'hover' : FALSE,
          ),
        ),
      ),
    ),
  ), 'setting');

  backdrop_add_js(backdrop_get_path('module', 'views_bootstrap') . '/js/views-bootstrap-carousel.js');

  if ($vars['options']['items_per_slide'] > 1) {
    $rows = array();

    foreach ($vars['rows'] as $key => $row) {
      $mod = $key % (int) $vars['options']['items_per_slide'];
      if ($mod == 0) {
        $rows[] = $row;
      }
      else {
        $rows[(count($rows)-1)] .= $row;
      }
    }

    $vars['rows'] = $rows;
  }

  $vars['classes_array'][] = 'carousel slide';
  $vars['attributes_array']['data-ride'] = 'carousel';

  $vars['navigation'] = (bool)$vars['options']['navigation'];
  $vars['indicators'] = (bool)$vars['options']['indicators'];
  $keys = array_keys($vars['rows']);
  $vars['first_key'] = reset($keys);
}

/**
 * Implementation of template preprocess for the view fields.
 */
function template_preprocess_views_bootstrap_carousel_plugin_rows(&$vars) {
  $view = &$vars['view'];
  $row_index = $view->row_index; // Clone value because get_field will unset it.

  foreach ($vars['options'] as $id => $field) {
    if (isset($view->field[$field])) {
      $vars[$id] = $view->style_plugin->get_field($row_index, $field);
    }
  }
}

/**
 * Implementation of template preprocess for the view.
 */
function template_preprocess_views_bootstrap_grid_plugin_style(&$vars) {
  _views_bootstrap_include_bootstrap_assets();

  $view     = $vars['view'];
  $options  = $view->style_plugin->options;
  $horizontal = ($options['alignment'] === 'horizontal');

  $columns = $options['columns'];

  // Split items by rows and columns.
  $vars['items'] = _views_bootstrap_split_rows($vars, $columns, $horizontal);
  $vars['column_type'] = 12 / $columns;
  $vars['class_prefix'] = $options['class_prefix'] ?: 'col-sm';
}


/**
 * Implementation of template preprocess for the view.
 */
function template_preprocess_views_bootstrap_list_group_plugin_style(&$vars) {
  _views_bootstrap_include_bootstrap_assets();

  $view = $vars['view'];
  $link_field = $vars['options']['link_field'];

  $vars['linked_items'] = !empty($link_field);

  foreach (array_keys($vars['rows']) as $key) {
    $vars['link_fields'][$key] = $view->style_plugin->get_field($key, $link_field);
  }
}


/**
 * Implementation of template preprocess for the view.
 */
function template_preprocess_views_bootstrap_media_plugin_style(&$vars) {
  _views_bootstrap_include_bootstrap_assets();

  $view = &$vars['view'];

  $image_field = $vars['options']['image_field'];
  $heading_field = $vars['options']['heading_field'];
  $body_field = $vars['options']['body_field'];

  $vars['items'] = array();

  foreach (array_keys($vars['rows']) as $key) {
    $vars['items'][] = array(
      'image_field' => isset($view->field[$image_field]) ? $view->style_plugin->get_field($key, $image_field) : NULL,
      'heading_field' => isset($view->field[$heading_field]) ? $view->style_plugin->get_field($key, $heading_field) : NULL,
      'body_field' => isset($view->field[$body_field]) ? $view->style_plugin->get_field($key, $body_field) : NULL,
    );
  }
}


/**
 * Implementation of template preprocess for the view.
 */
function template_preprocess_views_bootstrap_tab_plugin_style(&$vars) {
  _views_bootstrap_include_bootstrap_assets();

  $view = &$vars['view'];
  $tab_field = $vars['options']['tab_field'];

  $vars['tab_type'] = $vars['options']['tab_type'];
  $vars['justified'] = $vars['options']['justified'];

  // Get tabs.
  if (isset($view->field[$tab_field])) {
    foreach (array_keys($vars['rows']) as $key) {
      $vars['tabs'][$key] = $view->style_plugin->get_field($key, $tab_field);
    }
  }

  $keys = array_keys($vars['rows']);
  $vars['first_key'] = reset($keys);
}



/**
 * Implementation of template preprocess for the view.
 */
function template_preprocess_views_bootstrap_table_plugin_style(&$vars) {
  _views_bootstrap_include_bootstrap_assets();

  template_preprocess_views_view_table($vars);

  $vars['responsive'] = $vars['options']['responsive'];

  $vars['classes_array'][] = 'table';

  foreach ($vars['options']['bootstrap_styles'] as $style) {
    $vars['classes_array'][] = 'table-' . $style;
  }
}


/**
 * Implementation of template preprocess for the view.
 */
function template_preprocess_views_bootstrap_thumbnail_plugin_style(&$vars) {
  _views_bootstrap_include_bootstrap_assets();

  $view     = $vars['view'];
  $options  = $view->style_plugin->options;
  $horizontal = ($options['alignment'] === 'horizontal');

  $columns = $options['columns'];

  // Split items by rows and columns.
  $vars['items'] = _views_bootstrap_split_rows($vars, $columns, $horizontal);
  $vars['column_type'] = 12 / $columns;
  $vars['class_prefix'] = $options['class_prefix'] ?: 'col-sm';
}

/**
 * Implementation of template preprocess for the view fields.
 */
function template_preprocess_views_bootstrap_thumbnail_plugin_rows(&$vars) {
  $view = &$vars['view'];
  $row_index = $view->row_index; // Clone value because get_field will unset it.

  foreach ($vars['options'] as $id => $field) {
    switch ($id) {
      case 'image':
      case 'title':
        if (isset($view->field[$field])) {
          $vars[$id] = $view->style_plugin->get_field($row_index, $field);
        }
        break;

      case 'content':
        $vars[$id] = array();
        foreach (array_filter($field) as $content_field) {
          if (isset($view->field[$content_field])) {
            $vars[$id][$content_field] = $view->style_plugin->get_field($row_index, $content_field);
          }
        }
        break;
    }
  }
}
