<?php

/**
 * @file
 * Views integration for Commerce Card on File.
 */

/**
 * Implements hook_views_data()
 */
function commerce_cardonfile_views_data() {
  $data = array();

  // -----------------------------------------------------------------------
  // Base table setup

  $data['commerce_card_data']['table']['group']  = t('Commerce Card on File');
  $data['commerce_card_data']['table']['base'] = array(
    'field' => 'id',
    'title' => t('Commerce Card on File'),
    'help' => t('Saves information about each saved card data.'),
  );

  // For other base tables, explain how we join
  // 'left_field' is the primary key in the referenced table.
  // 'field' is the foreign key in this table.
  $data['commerce_card_data']['table']['join'] = array(
    'users' => array(
      'left_field' => 'uid',
      'field' => 'uid',
    ),
  );

  // Expose Fields

  // Card ID.
  $data['commerce_card_data']['card_id'] = array(
    'title' => t('Card ID'),
    'help' => t('The unique internal identifier of the card data.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
    'relationship' => array(
      'handler' => 'views_handler_relationship',
      'base' => 'commerce_card_data',
      'field' => 'card_id',
      'label' => t('Card Data', array(), array('context' => 'credit card data')),
    ),
  );

  // Card owner uid.
  $data['commerce_card_data']['uid'] = array(
    'title' => t('Uid'),
    'help' => t("The owner's user ID."),
    'field' => array(
      'handler' => 'views_handler_field_user',
      'click sortable' => TRUE,
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_user_uid',
      'name field' => 'name', // display this field in the summary
    ),
    'filter' => array(
      'title' => t('Name'),
      'handler' => 'views_handler_filter_user_name',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'relationship' => array(
      'title' => t('Owner'),
      'help' => t("Relate this card to its owner's user account"),
      'handler' => 'views_handler_relationship',
      'base' => 'users',
      'base field' => 'uid',
      'field' => 'uid',
      'label' => t('Card owner'),
    ),
  );

  // Card payment method
  $data['commerce_card_data']['payment_method'] = array(
    'title' => t('Payment method'),
    'help' => t('The payment method related to this card.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  // Card instance id
  $data['commerce_card_data']['instance_id'] = array(
    'title' => t('Payment method instance ID'),
    'help' => t('The payment method instance ID related to this card.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  // Card remote id
  $data['commerce_card_data']['remote_id'] = array(
    'title' => t('Card data remote ID'),
    'help' => t('The remote ID to the full card data at the payment gateway.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  // Card card_type
  $data['commerce_card_data']['card_type'] = array(
    'title' => t('Card type'),
    'help' => t('The credit card type.'),
    'field' => array(
      'handler' => 'commerce_cardonfile_handler_field_card_type',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  // Card card_name
  $data['commerce_card_data']['card_name'] = array(
    'title' => t('Name on the card'),
    'help' => t('The name on the credit card.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  // Card card_number
  $data['commerce_card_data']['card_number'] = array(
    'title' => t('Card number'),
    'help' => t('Truncated number of the credit card (last 4 digits).'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  // Card expiration month
  $data['commerce_card_data']['card_exp_month'] = array(
    'title' => t('Card expiration month'),
    'help' => t('Expiration month of the credit card.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
  );

  // Card expiration year
  $data['commerce_card_data']['card_exp_year'] = array(
    'title' => t('Card expiration year'),
    'help' => t('Expiration year of the credit card.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
  );

  // Card status
  $data['commerce_card_data']['status'] = array(
    'title' => t('Card data status'),
    'help' => t('Status of the card data storage.'),
    'field' => array(
      'handler' => 'commerce_cardonfile_handler_field_card_status',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'commerce_cardonfile_handler_filter_card_status',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
  );

  // Card created timestamp
  $data['commerce_card_data']['created'] = array(
    'title' => t('Created'),
    'help' => t('The date when this card was created.'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
  );

  // Card created timestamp
  $data['commerce_card_data']['changed'] = array(
    'title' => t('Changed'),
    'help' => t('The date when this card was changed.'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
  );

  // Default card
  $data['commerce_card_data']['instance_default'] = array(
    'title' => t('Default card'),
    'help' => t('Whether or not this is the default card for the Payment Instance ID..'),
    'field' => array(
      'handler' => 'views_handler_field_boolean',
      'click sortable' => TRUE,
      'output formats' => array(
        'boolean' => array(1, 0),
      ),
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_boolean_operator',
      'label' => t('Default'),
      'type' => 'yes-no',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // Operation links
  $data['commerce_card_data']['operations'] = array(
    'field' => array(
      'title' => t('Operations links'),
      'help' => t('Display all the available operations links for the cards.'),
      'handler' => 'commerce_cardonfile_handler_field_data_operations',
    ),
  );


  return $data;
}

/**
 * Implements hook_views_plugins
 */
function commerce_cardonfile_views_plugins() {
  return array(
    'argument validator' => array(
      'commerce_cof_card_id' => array(
        'title' => t('Card ID access'),
        'handler' => 'commerce_cardonfile_plugin_argument_validate_card_id',
      ),
      'commerce_cof_owner' => array(
        'title' => t('Card owner access'),
        'handler' => 'commerce_cardonfile_plugin_argument_validate_owner',
      ),
    ),
  );
}
