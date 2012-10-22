<?php

/**
 * @file
 * Defines a field type for referencing card on file data from other entities.
 */

/**
 * Implements hook_field_info().
 */
function commerce_cardonfile_field_info() {
  return array(
    'commerce_cardonfile_reference' => array(
      'label' => t('Card on file reference'),
      'description' => t('This field stores the ID of a related card on file as an integer value.'),
      'settings' => array(),
      'instance_settings' => array(
        'only_active_cards' => TRUE,
        'include_disabled_payment_instances' => FALSE,
        'referenceable_payment_instances' => array(),
      ),
      'default_widget' => 'options_select',
      'default_formatter' => 'commerce_cardonfile_reference_label_link',
      'property_type' => 'commerce_cardonfile',
      'property_callbacks' => array('commerce_cardonfile_reference_property_info_callback'),
      'default_token_formatter' => 'commerce_cardonfile_reference_label_plain',
    ),
  );
}

/**
 * Implements hook_field_instance_settings_form().
 */
function commerce_cardonfile_field_instance_settings_form($field, $instance) {
  $settings = $instance['settings'];
  $settings += field_info_instance_settings($field['type']);

  $form = array();

  $form['only_active_cards'] = array(
    '#type' => 'checkbox',
    '#title' => t('Only active cards.'),
    '#description' => t('If enabled, only active cards will be allowed.'),
    '#default_value' => $settings['only_active_cards'],
  );

  $form['include_disabled_payment_instances'] = array(
    '#type' => 'checkbox',
    '#title' => t('Allow disabled payment method instances.'),
    '#description' => t('If enabled, cards associated with a disabled payment method instance will be allowed.'),
    '#default_value' => $settings['include_disabled_payment_instances'],
  );

  // Build an options array of the payment method instances.
  $options = array();

  $payment_instances = _commerce_cardonfile_capable_payment_method_instances(TRUE);
  foreach ($payment_instances as $payment_instance_id => $payment_instance) {
    // load payment rule
    list($payment_method_id, $payment_instance_rule_name) = explode('|', $payment_instance_id);
    $payment_rule = rules_config_load($payment_instance_rule_name);

    // Set option and label
    $options[$payment_instance_id] = t('@title (!status)', array(
      '@title' => $payment_instance['title'],
      '!status' => !empty($payment_rule->active) ? t('Enabled') : t('Disabled'),
    ));
  }

  $form['referenceable_payment_instances'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Payment method instances that can be referenced'),
    '#description' => t('If none are selected, any payment instance may be referenced.'),
    '#options' => $options,
    '#default_value' => is_array($settings['referenceable_payment_instances']) ? $settings['referenceable_payment_instances'] : array(),
    '#multiple' => TRUE,
    //'#weight' => -3,
  );

  return $form;
}

/**
 * Implements hook_field_validate().
 *
 * Possible error codes:
 * - 'invalid_card_id': card_id is not valid for the field (not a valid
 *                         card id, or the card is not referenceable).
 */
function commerce_cardonfile_field_validate($entity_type, $entity, $field, $instance, $langcode, $items, &$errors) {
  // Extract card_ids to check.
  $card_ids = array();

  // First check non-numeric card_id's to avoid losing time with them.
  foreach ($items as $delta => $item) {
    if (is_array($item) && !empty($item['card_id'])) {
      if (is_numeric($item['card_id'])) {
        $card_ids[] = $item['card_id'];
      }
      else {
        $errors[$field['field_name']][$langcode][$delta][] = array(
          'error' => 'invalid_card_id',
          'message' => t('%name: you have specified an invalid card for this reference field.', array('%name' => $instance['label'])),
        );
      }
    }
  }

  // Prevent performance hog if there are no ids to check.
  if ($card_ids) {
/** @todo: should validate only allow active cards? *********/
    $refs = commerce_cardonfile_data_load_multiple_by_id($card_ids);

    foreach ($items as $delta => $item) {
      if (is_array($item)) {
        if (!empty($item['card_id']) && !isset($refs[$item['card_id']])) {
          $errors[$field['field_name']][$langcode][$delta][] = array(
            'error' => 'invalid_card_id',
            'message' => t('%name: you have specified an invalid card for this reference field.', array('%name' => $instance['label'])),
          );
        }
      }
    }
  }
}

/**
 * Implements hook_field_is_empty().
 */
function commerce_cardonfile_field_is_empty($item, $field) {
  // card_id = 0 is empty too, which is exactly what we want.
  return empty($item['card_id']);
}

/**
 * Implements hook_field_formatter_info().
 */
function commerce_cardonfile_field_formatter_info() {
  return array(
    'commerce_cardonfile_reference_label_link' => array(
      'label' => t('Label (link)'),
      'description' => t('Display the label of the referenced card as a link to the edit page (if allowed).'),
      'field types' => array('commerce_cardonfile_reference'),
    ),
    'commerce_cardonfile_reference_label_plain' => array(
      'label' => t('Label (no link)'),
      'description' => t('Display the label of the referenced card as plain text.'),
      'field types' => array('commerce_cardonfile_reference'),
    ),
    'commerce_cardonfile_reference_overview' => array(
      'label' => t('Overview'),
      'description' => t('Display a themed overview of the referenced card.'),
      'field types' => array('commerce_cardonfile_reference'),
    ),
  );
}

/**
 * Implements hook_field_formatter_view().
 */
function commerce_cardonfile_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  $result = array();

  // Collect the list of card IDs.
  $card_ids = array();

  foreach ($items as $delta => $item) {
    $card_ids[$item['card_id']] = $item['card_id'];
  }

  // Exit now if we didn't find any card IDs.
  if (empty($card_ids)) {
    return;
  }

  // Load the referenced cards.
/** @todo: only show active?? *******/
  $cards = commerce_cardonfile_data_load_multiple_by_id($card_ids);

  switch ($display['type']) {
    case 'commerce_cardonfile_reference_label_link':
    case 'commerce_cardonfile_reference_label_plain':
      foreach ($items as $delta => $item) {
        if (!empty($cards[$item['card_id']])) {
          $card = $cards[$item['card_id']];

          if ($display['type'] == 'commerce_cardonfile_reference_label_link' &&
              commerce_cardonfile_data_access('edit', $card)) {
            $label = theme('commerce_cardonfile_data_label', array('card_data' => $card));
            $result[$delta] = array(
              '#type' => 'link',
              '#title' => $label,
              '#href' => 'user/' . $card['uid'] . '/stored-payment-methods/' . $card['card_id'] . '/update',
            );
          }
          else {
            $result[$delta] = array(
              '#theme' => 'commerce_cardonfile_data_label',
              '#card_data' => $card,
            );
          }
        }
      }
      break;

    case 'commerce_cardonfile_reference_overview':
      foreach ($items as $delta => $item) {
        if (!empty($cards[$item['card_id']])) {
          $result[$delta] = array(
            '#theme' => 'card_data_overview',
            '#card_data' => $cards[$item['card_id']],
          );
        }
      }
      break;
  }

  return $result;
}

/**
 * Implements hook_field_widget_info().
 *
 * Defines widgets available for use with field types as specified in each
 * widget's $info['field types'] array.
 */
function commerce_cardonfile_field_widget_info() {
  $widgets = array();

  // Do not show the widget on forms; useful in cases where reference fields
  // have a lot of data that is maintained automatically.
  $widgets['commerce_cardonfile_reference_hidden'] = array(
    'label' => t('Do not show a widget'),
    'description' => t('Will not display the card reference field on forms. Use only if you maintain card references some other way.'),
    'field types' => array('commerce_cardonfile_reference'),
    'behaviors' => array(
      'multiple values' => FIELD_BEHAVIOR_CUSTOM,
    ),
  );

  return $widgets;
}

/**
 * Implements hook_field_widget_info_alter().
 */
function commerce_cardonfile_field_widget_info_alter(&$info) {
  $info['options_select']['field types'][] = 'commerce_cardonfile_reference';
  $info['options_buttons']['field types'][] = 'commerce_cardonfile_reference';
}

/**
 * Implements hook_field_widget_settings_form().
 */
function commerce_cardonfile_field_widget_settings_form($field, $instance) {
  $widget = $instance['widget'];
  $defaults = field_info_widget_settings($widget['type']);
  $settings = array_merge($defaults, $widget['settings']);

  $form = array();

  return $form;
}

/**
 * Implements hook_field_widget_form().
 *
 * Used to define the form element for custom widgets.
 */
function commerce_cardonfile_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {
  // Hide the widget
  if ($instance['widget']['type'] == 'commerce_cardonfile_reference_hidden') {
    return array();
  }
}

/**
 * Implements hook_field_widget_error().
 */
function commerce_cardonfile_field_widget_error($element, $error) {
  form_error($element, $error['message']);
}

/**
 * Implements hook_options_list().
 */
function commerce_cardonfile_options_list($field, $instance, $entity_type, $entity) {
  $options = array();
  if ($field['type'] != 'commerce_cardonfile_reference') {
    return array();
  }

  $settings = !empty($instance['settings']) ? $instance['settings'] : array();
  $settings += field_info_instance_settings($field['type']);

  // resolve owner
  if (empty($entity->uid)) {
    return array();
  }

  // build options based on payment method instances
  $payment_instance_ids = array();
  if ($entity_type == 'commerce_order' && !empty($entity->data['payment_method'])) {
    // set to only payment method instance associated with the order
/** @todo should the options be restricted based on the order payment method ? ******/
    $payment_instance_ids[] = $entity->data['payment_method'];
  }
  else {
    // set to all instances
    $payment_instances = _commerce_cardonfile_capable_payment_method_instances(!empty($settings['include_disabled_payment_instances']));
    if (!empty($payment_instances)) {
      $payment_instance_ids = array_keys($payment_instances);
    }
  }

  // exit if no payment instances
  if (empty($payment_instance_ids)) {
    return array();
  }

  // filter payment instances by referenceable instances
  if (!empty($settings['referenceable_payment_instances'])) {
    $payment_instance_ids = array_diff($payment_instance_ids, $settings['referenceable_payment_instances']);
  }

  // build query for cards
  $card_conditions = array(
    'uid' => $entity->uid,
    'instance_id' => $payment_instance_ids,
  );

  // filter by active status
  if (!empty($settings['only_active_cards'])) {
    $card_conditions['status'] = 1;
  }

  // execute query
  $available_cards = commerce_cardonfile_data_load_multiple_by_id(array(), $card_conditions);
  if (empty($available_cards)) {
    return array();
  }

  // build the options for each card
  foreach ($available_cards as $card_id => $card_data) {
    // Add them to the options list in optgroups by payment instance id.
    $payment_instance_id = check_plain($card_data['instance_id']);
    $label = $card_id;
    $replacements = array(
      '!default' => !empty($card_data['instance_default']) ? t('(user\'s default)') : '',
    );
    if (!empty($instance['widget']['type']) && $instance['widget']['type'] == 'options_select') {
      $replacements += _commerce_cardonfile_data_card_replacements($card_data, '!');
      $label = t('!card_type ***!card_number, Expires !card_exp_month/!card_exp_year !default', $replacements);
    }
    else {
      $replacements += _commerce_cardonfile_data_card_replacements($card_data, '@');
      $label = t('@card_type ***@card_number, Expires @card_exp_month/@card_exp_year !default', $replacements);
    }

    $options[$payment_instance_id][$card_id] = $label;
  }

  // Simplify the options list if only one optgroup exists.
  if (count($options) == 1) {
    $options = reset($options);
  }

  return $options;
}

/**
 * Callback to alter the property info of the reference fields.
 *
 * @see commerce_cardonfile_field_info().
 */
function commerce_cardonfile_reference_property_info_callback(&$info, $entity_type, $field, $instance, $field_type) {
  $property = &$info[$entity_type]['bundles'][$instance['bundle']]['properties'][$field['field_name']];
  $property['options list'] = 'entity_metadata_field_options_list';
}

/**
 * Implements hook_field_access().
 */
function commerce_cardonfile_field_access($op, $field, $entity_type, $entity, $account) {
  if ($field['type'] != 'commerce_cardonfile_reference' || empty($field['field_name']) || empty($entity)) {
    return;
  }

  $entity_wrapper = entity_metadata_wrapper($entity_type, $entity);
  if (!isset($entity_wrapper->{$field['field_name']})) {
    return;
  }

  $field_items = $entity_wrapper->{$field['field_name']}->value();
  if (!is_array($field_items)) {
    $field_items = array($field_items);
  }

  $card_ids = array();
  foreach ($field_items as $field_item_value) {
    $card_ids[] = $field_item_value;
  }

  // DENY if no view access to any of the cards
  if (!empty($card_ids)) {
    $cards = commerce_cardonfile_data_load_multiple_by_id($card_ids);
    foreach ($cards as $card_id => $card) {
      if (!commerce_cardonfile_data_access('view', $card)) {
        return FALSE;
      }
    }
  }
}

/**
 * Implements hook_field_views_data().
 */
function commerce_cardonfile_field_views_data($field) {
  if ($field['type'] != 'commerce_cardonfile_reference') {
    return;
  }

  $data = field_views_field_default_views_data($field);

  // Build an array of bundles the field appears on.
  $bundles = array();

  foreach ($field['bundles'] as $entity => $entity_bundles) {
    $bundles[] = $entity . ' (' . implode(', ', $entity_bundles) . ')';
  }

  foreach ($data as $table_name => $table_data) {
    foreach ($table_data as $field_name => $field_data) {
      if (isset($field_data['filter']['field_name']) && $field_name != 'delta') {
        $data[$table_name][$field_name]['relationship'] = array(
          'title' => t('Referenced card on file'),
          'help' => t('Appears in: @bundles.', array('@bundles' => implode(', ', $bundles))),
          'base' => 'commerce_card_data',
          'base field' => 'card_id',
          'handler' => 'views_handler_relationship',
          'label' => t('Card on File'),
        );
      }
    }
  }

  return $data;
}
