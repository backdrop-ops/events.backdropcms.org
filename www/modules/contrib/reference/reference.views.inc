<?php
/**
 * @file
 * Views integration for Reference.
 */

/**
 * Implements hook_field_views_data().
 */
function reference_field_views_data($field) {
  // Get info for the entity type this field references to add a relationship.
  $target_info = entity_get_info($field['settings']['entity_type']);
  // Get the default values of the views config data for this field.
  $data = field_views_field_default_views_data($field);

  $entity_type = $target_info['label'];
  if ($entity_type == t('Node')) {
    $entity_type = t('Content');
  }
  $parameters = array('@type' => $entity_type, '!field_name' => $field['field_name']);

  // Alter data for both the field_data and field_revision tables.
  foreach ($data as $table_name => $table_data) {
    // Add a relationship to the target table from the [field_name]_target_id.
    $data[$table_name][$field['field_name'] . '_target_id']['relationship'] = array(
      'handler' => 'views_handler_relationship',
      'base' => $target_info['base table'],
      'base field' => $target_info['entity keys']['id'],
      'label' => t('@type referenced by !field_name', $parameters),
      'group' => t('Reference'),
      'title' => t('Reference to'),
      'help' => t('A bridge to the @type that is referenced in the field !field_name', $parameters),
    );
  }
  return $data;
}

/**
 * Implements hook_field_views_data_views_data_alter().
 *
 * Views integration to provide reverse relationships on reference fields.
 */
function reference_field_views_data_views_data_alter(&$data, $field) {
  foreach ($field['bundles'] as $entity_type => $bundles) {
    if (isset($field['settings']['target_type'])) {
      $target_entity_info = entity_get_info($field['settings']['target_type']);
      if (isset($target_entity_info['base table'])) {
        $entity_info = entity_get_info($entity_type);
        $entity = $entity_info['label'];
        if ($entity == t('Node')) {
          $entity = t('Content');
        }
        $target_entity = $target_entity_info['label'];
        if ($target_entity == t('Node')) {
          $target_entity = t('Content');
        }

        $pseudo_field_name = 'reverse_' . $field['field_name'] . '_' . $entity_type;
        $replacements = array('@entity' => $entity, '@target_entity' => $target_entity, '!field_name' => $field['field_name']);
        $data[$target_entity_info['base table']][$pseudo_field_name]['relationship'] = array(
          'handler' => 'views_handler_relationship_entity_reverse',
          'field_name' => $field['field_name'],
          'field table' => _field_sql_storage_tablename($field),
          'field field' => $field['field_name'] . '_target_id',
          'base' => $entity_info['base table'],
          'base field' => $entity_info['entity keys']['id'],
          'label' => t('@entity referencing @target_entity from !field_name', $replacements),
          'group' => t('Entity Reference'),
          'title' => t('Reference'),
          'help' => t('A bridge to the @entity that is referencing @target_entity via !field_name', $replacements),
          'join_extra' => array(
            0 => array(
              'field' => 'entity_type',
              'value' => $entity_type,
            ),
            1 => array(
              'field' => 'deleted',
              'value' => 0,
              'numeric' => TRUE,
            ),
          ),
        );
      }
    }
  }
}
