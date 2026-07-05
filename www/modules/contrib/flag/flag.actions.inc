<?php

/**
 * @file
 * Callbacks for flag actions.
 */


/**
 * Flags an entity.
 *
 * @param Entity $entity
 *   The entity object.
 * @param array $context
 *   Context array.
 *
 * @ingroup actions
 */
function flag_flag_entity_action(&$entity, $context = array()) {
  $entity_flags = flag_get_flags($context['action_info']['type']);
  $flag_name = $context['action_info']['flag_name'];
  $flag = $entity_flags[$flag_name];
  $account = isset($context['account']) ? $context['account'] : $GLOBALS['user'];
  if (!$flag->is_flagged($entity->id())) {
    $flag->flag('flag', $entity->id(), $account, TRUE);
  }
  else {
    return FALSE;
  }
}

/**
 * Flags an entity.
 *
 * @param Entity $entity
 *   The entity object.
 * @param array $context
 *   Context array.
 *
 * @ingroup actions
 */
function flag_unflag_entity_action(&$entity, $context = array()) {
  $entity_flags = flag_get_flags($context['action_info']['type']);
  $flag_name = $context['action_info']['flag_name'];
  $flag = $entity_flags[$flag_name];
  $account = isset($context['account']) ? $context['account'] : $GLOBALS['user'];
  if ($flag->is_flagged($entity->id())) {
    $flag->flag('unflag', $entity->id(), $account, TRUE);
  } else {
    return FALSE;
  }
}
