
<?php
/**
 * @file
 * Theme function overrides.
 */

/*******************************************************************************
 * Alter functions: modify renderable structures before used.
 ******************************************************************************/


/*******************************************************************************
 * Preprocess functions: prepare variables for templates.
 ******************************************************************************/

/**
 * Prepares variables for views grid templates.
 * @see views-view-grid.tpl.php
 */
function borg_events_preprocess_views_view_grid(&$variables) {
  $view = $variables['view'];
    $result = $view->result;
    $options = $view->style_plugin->options;
    $handler = $view->style_plugin;
    $default_row_class = isset($options['default_row_class']) ? $options['default_row_class'] : TRUE;
    $row_class_special = isset($options['row_class_special']) ? $options['row_class_special'] : TRUE;

    $columns = $options['columns'];

    $rows = array();
    $row_indexes = array();

    // For old view default to table if view settings haven't been updated.
    $variables['deprecated_table'] = (!isset($options['deprecated_table']) || $options['deprecated_table']) ? TRUE : FALSE;

    if (!$variables['deprecated_table']) {
      $rows = $variables['rows'];

      // Set up striping values.
      $count = 0;
      $max = count($rows);

      // Reorder based on vertical alignment.
      if ($options['alignment'] == 'vertical') {
        $rows = array();
        $rows_reorder = array();
        $num_rows = floor(count($variables['rows']) / $columns);
        // The remainders are the 'odd' columns that are slightly longer.
        $remainders = count($variables['rows']) % $columns;
        $row = 0;
        $col = 0;

        foreach ($variables['rows'] as $count => $item) {
          $rows_reorder[$row][$col] = $item;

          $row_indexes[$row][$col] = $count;

          $row++;

          if (!$remainders && $row == $num_rows) {
            $row = 0;
            $col++;
          }
          elseif ($remainders && $row == $num_rows + 1) {
            $row = 0;
            $col++;
            $remainders--;
          }
        }

        // Flatten now that items are reordered.
        $col = 0;
        foreach ($rows_reorder as $row) {
          foreach ($row as $column) {
            $rows[$col] = $column;
            $col++;
          }
        }

      }

      $count = 0;
      $col_count = 1;
      foreach ($rows as $id => $row) {
        $count++;
        $variables['row_classes'][$id] = array();
        if ($default_row_class) {
          $variables['row_classes'][$id][] = 'views-grid-box';
          $variables['row_classes'][$id][] = 'views-grid-box-' . $count;
        }
        if ($row_class_special) {
          $variables['row_classes'][$id][] = ($count % 2 ? 'odd' : 'even');
          if ($count == 1) {
            $variables['row_classes'][$id][] = 'first';
          }
          if ($count == $max) {
            $variables['row_classes'][$id][] = 'last';
          }
          if ($col_count == 1) {
            $variables['row_classes'][$id][] = 'row-first';
          }
          if ($col_count == $options['columns']) {
            $variables['row_classes'][$id][] = 'row-last';
          }
        }
        if ($row_class = $view->style_plugin->get_row_class($id)) {
          $variables['row_classes'][$id][] = $row_class;
        }
        if ($col_count == $options['columns']) {
          $col_count = 1;
        }
        else {
          $col_count++;
        }
      }
      $variables['classes'] = array('views-view-grid', 'views-view-grid-cols-' . $columns);
    }
    else {
      if ($options['alignment'] == 'horizontal') {
        $row = array();
        $col_count = 0;
        $row_count = 0;
        $count = 0;
        foreach ($variables['rows'] as $row_index => $item) {
          $count++;
          $row[] = $item;
          $row_indexes[$row_count][$col_count] = $row_index;
          $col_count++;
          if ($count % $columns == 0) {
            $rows[] = $row;
            $row = array();
            $col_count = 0;
            $row_count++;
          }
        }
        if ($row) {
          // Fill up the last line only if it's configured, but this is default.
          if (!empty($handler->options['fill_single_line']) && count($rows)) {
            for ($i = 0; $i < ($columns - $col_count); $i++) {
              $row[] = '';
            }
          }
          $rows[] = $row;
        }
      }
      else {
        $num_rows = floor(count($variables['rows']) / $columns);
        // The remainders are the 'odd' columns that are slightly longer.
        $remainders = count($variables['rows']) % $columns;
        $row = 0;
        $col = 0;
        foreach ($variables['rows'] as $count => $item) {
          $rows[$row][$col] = $item;
          $row_indexes[$row][$col] = $count;
          $row++;

          if (!$remainders && $row == $num_rows) {
            $row = 0;
            $col++;
          }
          elseif ($remainders && $row == $num_rows + 1) {
            $row = 0;
            $col++;
            $remainders--;
          }
        }
        for ($i = 0; $i < count($rows[0]); $i++) {
          // This should be string so that's okay :)
          if (!isset($rows[count($rows) - 1][$i])) {
            $rows[count($rows) - 1][$i] = '';
          }
        }
      }

      // Apply the row classes
      foreach ($rows as $row_number => $row) {
        $row_classes = array();
        if ($default_row_class) {
          $row_classes[] = 'row-' . ($row_number + 1);
        }
        if ($row_class_special) {
          if ($row_number == 0) {
            $row_classes[] = 'row-first';
          }
          if (count($rows) == ($row_number + 1)) {
            $row_classes[] = 'row-last';
          }
        }
        $variables['row_classes'][$row_number] = $row_classes;
        foreach ($rows[$row_number] as $column_number => $item) {
          $column_classes = array();
          if ($default_row_class) {
            $column_classes[] = 'col-' . ($column_number + 1);
          }
          if ($row_class_special) {
            if ($column_number == 0) {
              $column_classes[] = 'col-first';
            }
            elseif (count($rows[$row_number]) == ($column_number + 1)) {
              $column_classes[] = 'col-last';
            }
          }
          if (isset($row_indexes[$row_number][$column_number]) && $column_class = $view->style_plugin->get_row_class($row_indexes[$row_number][$column_number])) {
            $column_classes[] = $column_class;
          }
          $variables['column_classes'][$row_number][$column_number] = $column_classes;
        }
      }

      $variables['classes'] = array('views-view-grid', 'cols-' . $columns);

      // Add the summary to the list if set.
      if (!empty($handler->options['summary'])) {
        $variables['attributes'] = array('summary' => filter_xss_admin($handler->options['summary']));
      }

      // Add the caption to the list if set.
      if (!empty($handler->options['caption'])) {
        $variables['caption'] = filter_xss_admin($handler->options['caption']);
      }
      else {
        $variables['caption'] = '';
      }
    }

    $variables['rows'] = $rows;
}

/******************************************************************************
 * Theme function: Replace existng theme functions with our own.
 ******************************************************************************/

/**
 * Overrides theme_menu_tree().
 */
function borg_events_menu_tree__user_menu($variables) {
  $variables['attributes']['class'][] = 'closed';

  global $user;

  $output  = '<nav class="borg-greeting">';
  $output .= '  <ul class="borg-user-menu">';
  $output .= '    <li class=top>';

  if ($user->uid) {
    $greeting = t('Hi @name!', array('@name'  => $user->name));
    $output .= '      <a href="#" id="greeting" class="greeting">' . $greeting . '</a>';
  }
  else {
    $output .= '      <a href="#" id="greeting" class="greeting">' . t('Welcome!') . '</a>';
  }

  $output .= '      <ul' . backdrop_attributes($variables['attributes']) . '>' . $variables['tree'] . '</ul>';
  $output .= '    </li>';
  $output .= '  </ul>';

  $output .= '  <a class="icon" title="Find us on GitHub" href="https://github.com/backdrop/backdrop"><i class="fa fa-github fa-2x" aria-hidden="true"></i></a>';
  $output .= '  <a class="icon" title="Follow os on Twitter" href="https://twitter.com/backdropcms"><i class="fa fa-twitter fa-2x" aria-hidden="true"></i></a>';
  $output .= '  <a class="icon" title="Subscribe to our Newsletter" href="https://backdropcms.org/newsletter"><i class="fa fa-envelope fa-2x" aria-hidden="true"></i></a>';

  $output .= '</nav>';

  return $output;
}
