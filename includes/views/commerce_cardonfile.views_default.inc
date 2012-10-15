<?php

/**
 * @file
 * Views for the default Commerce Card on File.
 */

/**
 * Implements hook_views_default_views().
 */
function commerce_cardonfile_views_default_views() {
  $views = array();

// -----------------------------------------------------------------------
// User cards

  $view = new view();
  $view->name = 'commerce_user_cof';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'commerce_card_data';
  $view->human_name = 'User Cards on file';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Stored Cards';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'none';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '10';
  $handler->display->display_options['style_plugin'] = 'default';
  $handler->display->display_options['row_plugin'] = 'fields';
  /* Field: Commerce Card on File: Uid */
  $handler->display->display_options['fields']['uid']['id'] = 'uid';
  $handler->display->display_options['fields']['uid']['table'] = 'commerce_card_data';
  $handler->display->display_options['fields']['uid']['field'] = 'uid';
  $handler->display->display_options['fields']['uid']['exclude'] = TRUE;
  $handler->display->display_options['fields']['uid']['link_to_user'] = FALSE;
  /* Field: Commerce Card on File: Card ID */
  $handler->display->display_options['fields']['card_id']['id'] = 'card_id';
  $handler->display->display_options['fields']['card_id']['table'] = 'commerce_card_data';
  $handler->display->display_options['fields']['card_id']['field'] = 'card_id';
  $handler->display->display_options['fields']['card_id']['exclude'] = TRUE;
  /* Field: Commerce Card on File: Payment Method Instance ID */
  $handler->display->display_options['fields']['instance_id']['id'] = 'instance_id';
  $handler->display->display_options['fields']['instance_id']['table'] = 'commerce_card_data';
  $handler->display->display_options['fields']['instance_id']['field'] = 'instance_id';
  $handler->display->display_options['fields']['instance_id']['exclude'] = TRUE;
  /* Field: Commerce Card on File: Name on the card */
  $handler->display->display_options['fields']['card_name']['id'] = 'card_name';
  $handler->display->display_options['fields']['card_name']['table'] = 'commerce_card_data';
  $handler->display->display_options['fields']['card_name']['field'] = 'card_name';
  $handler->display->display_options['fields']['card_name']['label'] = '';
  $handler->display->display_options['fields']['card_name']['element_label_colon'] = FALSE;
  /* Field: Commerce Card on File: Card number */
  $handler->display->display_options['fields']['card_number']['id'] = 'card_number';
  $handler->display->display_options['fields']['card_number']['table'] = 'commerce_card_data';
  $handler->display->display_options['fields']['card_number']['field'] = 'card_number';
  $handler->display->display_options['fields']['card_number']['label'] = '';
  $handler->display->display_options['fields']['card_number']['alter']['alter_text'] = TRUE;
  $handler->display->display_options['fields']['card_number']['alter']['text'] = '******[card_number]';
  $handler->display->display_options['fields']['card_number']['element_label_colon'] = FALSE;
  /* Field: Commerce Card on File: Card type */
  $handler->display->display_options['fields']['card_type']['id'] = 'card_type';
  $handler->display->display_options['fields']['card_type']['table'] = 'commerce_card_data';
  $handler->display->display_options['fields']['card_type']['field'] = 'card_type';
  $handler->display->display_options['fields']['card_type']['label'] = 'Type';
  /* Field: Commerce Card on File: Card expiration month */
  $handler->display->display_options['fields']['card_exp_month']['id'] = 'card_exp_month';
  $handler->display->display_options['fields']['card_exp_month']['table'] = 'commerce_card_data';
  $handler->display->display_options['fields']['card_exp_month']['field'] = 'card_exp_month';
  $handler->display->display_options['fields']['card_exp_month']['label'] = '';
  $handler->display->display_options['fields']['card_exp_month']['exclude'] = TRUE;
  $handler->display->display_options['fields']['card_exp_month']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['card_exp_month']['separator'] = '';
  /* Field: Commerce Card on File: Card expiration year */
  $handler->display->display_options['fields']['card_exp_year']['id'] = 'card_exp_year';
  $handler->display->display_options['fields']['card_exp_year']['table'] = 'commerce_card_data';
  $handler->display->display_options['fields']['card_exp_year']['field'] = 'card_exp_year';
  $handler->display->display_options['fields']['card_exp_year']['label'] = 'Expiration';
  $handler->display->display_options['fields']['card_exp_year']['alter']['alter_text'] = TRUE;
  $handler->display->display_options['fields']['card_exp_year']['alter']['text'] = '[card_exp_month]/[card_exp_year]';
  $handler->display->display_options['fields']['card_exp_year']['separator'] = '';
  /* Field: Commerce Card on File: Changed */
  $handler->display->display_options['fields']['changed']['id'] = 'changed';
  $handler->display->display_options['fields']['changed']['table'] = 'commerce_card_data';
  $handler->display->display_options['fields']['changed']['field'] = 'changed';
  $handler->display->display_options['fields']['changed']['label'] = 'Last Updated';
  $handler->display->display_options['fields']['changed']['date_format'] = 'short';
  /* Field: Commerce Card on File: Card data status */
  $handler->display->display_options['fields']['status']['id'] = 'status';
  $handler->display->display_options['fields']['status']['table'] = 'commerce_card_data';
  $handler->display->display_options['fields']['status']['field'] = 'status';
  $handler->display->display_options['fields']['status']['label'] = 'Status';
  /* Field: Commerce Card on File: Operations links */
  $handler->display->display_options['fields']['operations']['id'] = 'operations';
  $handler->display->display_options['fields']['operations']['table'] = 'commerce_card_data';
  $handler->display->display_options['fields']['operations']['field'] = 'operations';
  $handler->display->display_options['fields']['operations']['label'] = '';
  $handler->display->display_options['fields']['operations']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['operations']['add_destination'] = 0;
  /* Contextual filter: Commerce Card on File: Uid */
  $handler->display->display_options['arguments']['uid']['id'] = 'uid';
  $handler->display->display_options['arguments']['uid']['table'] = 'commerce_card_data';
  $handler->display->display_options['arguments']['uid']['field'] = 'uid';
  $handler->display->display_options['arguments']['uid']['default_action'] = 'access denied';
  $handler->display->display_options['arguments']['uid']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['uid']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['uid']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['uid']['summary_options']['items_per_page'] = '25';
  $handler->display->display_options['arguments']['uid']['specify_validation'] = TRUE;
  $handler->display->display_options['arguments']['uid']['validate']['type'] = 'commerce_cof_owner';
  $handler->display->display_options['arguments']['uid']['validate']['fail'] = 'access denied';
  /* Filter criterion: Commerce Card on File: Card data status */
  $handler->display->display_options['filters']['status']['id'] = 'status';
  $handler->display->display_options['filters']['status']['table'] = 'commerce_card_data';
  $handler->display->display_options['filters']['status']['field'] = 'status';
  $handler->display->display_options['filters']['status']['operator'] = 'not in';
  $handler->display->display_options['filters']['status']['value'] = array(
    0 => '0',
  );

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options['defaults']['hide_admin_links'] = FALSE;
  $handler->display->display_options['path'] = 'user/%/stored-payment-methods';
  $handler->display->display_options['menu']['type'] = 'tab';
  $handler->display->display_options['menu']['title'] = 'Stored Payments';
  $handler->display->display_options['menu']['description'] = 'Edit or delete your stored payment methods.';
  $handler->display->display_options['menu']['weight'] = '20';
  $handler->display->display_options['menu']['name'] = 'management';
  $handler->display->display_options['menu']['context'] = 0;

  $views[$view->name] = $view;


  return $views;
}
