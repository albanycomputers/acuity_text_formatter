<?php

/**
 * @file
 * Acuity Text Formatter admin interface.
 */

/**
 * Main Admin settings form.
 * Displays global options and the list of custom replacements.
 */
function acuity_text_formatter_admin_form($form, &$form_state) {
  $config = config('acuity_text_formatter.settings');

  // --- 1. Global Punctuation Settings ---
  $form['global_settings'] = [
    '#type' => 'fieldset',
    '#title' => t('Global Punctuation Settings'),
    '#description' => t('These settings apply to both <strong>CamelCase</strong> and <strong>Intercapped</strong> modes.'),
    '#collapsible' => FALSE,
  ];

  $form['global_settings']['capitalize_after_hyphen'] = [
    '#type' => 'checkbox',
    '#title' => t('Capitalize after hyphen (-)'),
    '#default_value' => $config->get('capitalize_after_hyphen'),
  ];

  $form['global_settings']['capitalize_after_apostrophe'] = [
    '#type' => 'checkbox',
    '#title' => t('Capitalize after apostrophe (\') (excluding "’s ")'),
    '#default_value' => $config->get('capitalize_after_apostrophe'),
  ];

  $form['global_settings']['capitalize_after_brackets'] = [
    '#type' => 'checkbox',
    '#title' => t('Capitalize after opening bracket ('),
    '#default_value' => $config->get('capitalize_after_brackets'),
  ];

  $form['global_settings']['capitalize_after_period'] = [
    '#type' => 'checkbox',
    '#title' => t('Capitalize after period (.)'),
    '#default_value' => $config->get('capitalize_after_period'),
  ];

  // Submit button for the Global Settings part (Standard system settings behavior)
  $form['actions'] = ['#type' => 'actions'];
  $form['actions']['submit'] = [
    '#type' => 'submit',
    '#value' => t('Save global settings'),
  ];

  // Use system_settings_form_submit manually so we can append other stuff below
  $form['#submit'][] = 'system_settings_form_submit';


  // --- 2. Custom Replacements Section ---
  $form['replacements_wrapper'] = [
    '#type' => 'container',
    '#attributes' => ['class' => ['acuity-admin-section', 'custom-replacements']],
    '#weight' => 10, // Ensure it appears below the global settings
  ];

  $form['replacements_wrapper']['header'] = [
    '#markup' => '<h3>' . t('Custom Replacements') . '</h3>' .
                 '<p>' . t('Define specific words or acronyms to force-format (e.g., "usa" -> "USA"). These apply to both <strong>CamelCase</strong> and <strong>Intercapped</strong> modes.') . '</p>',
  ];

  // Get replacements. If empty, seed defaults.
  $replacements = $config->get('custom_replacements');

  if (empty($replacements)) {
    // Seed defaults for the user
    $defaults = [
      uniqid() => ['search' => 'uk', 'replace' => 'UK'],
      uniqid() => ['search' => 'usa', 'replace' => 'USA'],
      uniqid() => ['search' => 'llc', 'replace' => 'LLC'],
    ];
    // Save defaults immediately so they appear
    $config->set('custom_replacements', $defaults);
    $config->save();
    $replacements = $defaults;
  }

  $header = [
    t('Edit'),
    t('Search For (Input)'),
    t('Replace With (Output)'),
    t('Delete'),
  ];

  $rows = [];
  if (!empty($replacements)) {
    foreach ($replacements as $id => $data) {
      $rows[] = [
        l(t('Edit'), "admin/config/acuity-utils/acuity_text/edit/$id", [
          'attributes' => ['title' => t('Edit replacement')],
        ]),
        check_plain($data['search']),
        check_plain($data['replace']),
        l(t('Delete'), "admin/config/acuity-utils/acuity_text/delete/$id", [
          'attributes' => ['class' => ['abms-delete-link']],
        ]),
      ];
    }
  }

  $form['replacements_wrapper']['table'] = [
    '#markup' => theme('table', ['header' => $header, 'rows' => $rows, 'empty' => t('No replacements defined.')]),
  ];

  $form['replacements_wrapper']['add'] = [
    '#type' => 'link',
    '#title' => t('Add new replacement'),
    '#href' => 'admin/config/acuity-utils/acuity_text/add',
    '#attributes' => ['class' => ['button', 'button-action']],
  ];

  return $form;
}

/**
 * Form to Add or Edit a custom replacement.
 */
function acuity_text_formatter_edit_form($form, &$form_state, $id = NULL) {
  $config = config('acuity_text_formatter.settings');
  $replacements = $config->get('custom_replacements') ?: [];

  // Load existing data if ID is provided
  $data = ($id && isset($replacements[$id])) ? $replacements[$id] : [];

  $form['id'] = [
    '#type' => 'value',
    '#value' => $id,
  ];

  $form['search'] = [
    '#type' => 'textfield',
    '#title' => t('Search for'),
    '#description' => t('The text to look for (case-insensitive). e.g., "usa"'),
    '#default_value' => isset($data['search']) ? $data['search'] : '',
    '#required' => TRUE,
  ];

  $form['replace'] = [
    '#type' => 'textfield',
    '#title' => t('Replace with'),
    '#description' => t('The text to replace it with. e.g., "USA"'),
    '#default_value' => isset($data['replace']) ? $data['replace'] : '',
    '#required' => TRUE,
  ];

  $form['actions'] = ['#type' => 'actions'];
  $form['actions']['submit'] = [
    '#type' => 'submit',
    '#value' => t('Save'),
  ];

  $form['actions']['cancel'] = [
    '#type' => 'link',
    '#title' => t('Cancel'),
    '#href' => 'admin/config/acuity-utils/acuity_text',
    '#attributes' => ['class' => ['button', 'button-secondary']],
  ];

  return $form;
}

/**
 * Submit handler for adding/editing a replacement.
 */
function acuity_text_formatter_edit_form_submit($form, &$form_state) {
  $config = config('acuity_text_formatter.settings');
  $replacements = $config->get('custom_replacements') ?: [];

  $id = $form_state['values']['id'];

  // If no ID exists (Add mode), generate a unique ID
  if (empty($id)) {
    $id = uniqid();
  }

  // Update data
  $replacements[$id] = [
    'search' => trim($form_state['values']['search']),
    'replace' => trim($form_state['values']['replace']),
  ];

  $config->set('custom_replacements', $replacements);
  $config->save();

  backdrop_set_message(t('Custom replacement saved.'));
  $form_state['redirect'] = 'admin/config/acuity-utils/acuity_text';
}

/**
 * Confirmation form for deleting a replacement.
 */
function acuity_text_formatter_delete_form($form, &$form_state, $id) {
  $config = config('acuity_text_formatter.settings');
  $replacements = $config->get('custom_replacements');

  if (!isset($replacements[$id])) {
    backdrop_set_message(t('Replacement not found.'), 'error');
    backdrop_goto('admin/config/acuity-utils/acuity_text');
  }

  $item = $replacements[$id];

  $form['id'] = ['#type' => 'value', '#value' => $id];

  return confirm_form(
    $form,
    t('Are you sure you want to delete the replacement for "%search"?', ['%search' => $item['search']]),
    'admin/config/acuity-utils/acuity_text',
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Submit handler for deletion.
 */
function acuity_text_formatter_delete_form_submit($form, &$form_state) {
  $id = $form_state['values']['id'];
  $config = config('acuity_text_formatter.settings');
  $replacements = $config->get('custom_replacements');

  if (isset($replacements[$id])) {
    unset($replacements[$id]);
    $config->set('custom_replacements', $replacements);
    $config->save();
    backdrop_set_message(t('Replacement deleted.'));
  }

  $form_state['redirect'] = 'admin/config/acuity-utils/acuity_text';
}