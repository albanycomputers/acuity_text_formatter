<?php

/**
 * @file
 * Admin forms for Acuity Text Formatter.
 */

/**
 * Returns common case transform options.
 */
function _acuity_text_formatter_case_options(): array {
  return [
    'none' => t('- None -'),
    'lowercase' => t('Convert to lowercase'),
    'uppercase' => t('Convert to uppercase'),
    'camelcase' => t('CamelCase'),
    'intercapped' => t('Intercapped (e.g., McCallister, O\'Connor)'),
  ];
}

/**
 * Returns a readable summary of formatting rules.
 */
function _acuity_text_formatter_rule_summary(array $settings): string {
  $rules = [];

  if (!empty($settings['trim'])) {
    $rules[] = t('Trim');
  }

  if (!empty($settings['collapse_spaces'])) {
    $rules[] = t('Collapse spaces');
  }

  if (!empty($settings['case_transform']) && $settings['case_transform'] !== 'none') {
    $labels = [
      'lowercase' => t('Lowercase'),
      'uppercase' => t('Uppercase'),
      'camelcase' => t('CamelCase'),
      'intercapped' => t('Intercapped'),
    ];
    $rules[] = $labels[$settings['case_transform']] ?? $settings['case_transform'];
  }

  return empty($rules) ? t('No formatting enabled') : implode(', ', $rules);
}

/**
 * Global settings form.
 */
function acuity_text_formatter_global_form($form, &$form_state) {
  $config = config('acuity_text_formatter.settings');

  $form['#config'] = 'acuity_text_formatter.settings';

  $form['global_settings'] = [
    '#type' => 'fieldset',
    '#title' => t('Global Punctuation Settings'),
    '#description' => t('These settings apply to both <strong>CamelCase</strong> and <strong>Intercapped</strong> modes.'),
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

  $form['global_settings']['capitalize_after_newline'] = [
    '#type' => 'checkbox',
    '#title' => t('Capitalize start of new lines (Paragraphs)'),
    '#default_value' => $config->get('capitalize_after_newline'),
  ];

  $form['global_settings']['capitalize_mc'] = [
    '#type' => 'checkbox',
    '#title' => t('Apply Mc/Mac intercapped name logic'),
    '#default_value' => $config->get('capitalize_mc'),
  ];

  $form['actions'] = ['#type' => 'actions'];

  $form['actions']['submit'] = [
    '#type' => 'submit',
    '#value' => t('Save global settings'),
    '#button_type' => 'submit',
    '#attributes' => ['class' => ['button-primary']],
  ];

  return system_settings_form($form);
}

/**
 * Custom replacements listing form.
 */
function acuity_text_formatter_custom_replacements_form($form, &$form_state) {
  $config = config('acuity_text_formatter.settings');

  $form['description'] = [
    '#markup' => '<p>' . t('Define standalone words or acronyms to force-format (e.g., "uk" -> "UK"). Replacements are matched as whole words only. These apply to both <strong>CamelCase</strong> and <strong>Intercapped</strong> modes.') . '</p>',
  ];

  $replacements = $config->get('custom_replacements') ?: [];

  $header = [
    t('Edit'),
    t('Search for (input)'),
    t('Replace with (output)'),
    t('Delete'),
  ];

  $rows = [];

  foreach ($replacements as $id => $data) {
    $rows[] = [
      l(t('Edit'), "admin/config/acuity-utils/acuity_text/custom/edit/$id"),
      check_plain($data['search'] ?? ''),
      check_plain($data['replace'] ?? ''),
      l(t('Delete'), "admin/config/acuity-utils/acuity_text/custom/delete/$id", [
        'attributes' => ['class' => ['acuity-delete-link']],
      ]),
    ];
  }

  $form['table'] = [
    '#markup' => theme('table', [
      'header' => $header,
      'rows' => $rows,
      'empty' => t('No replacements defined.'),
    ]),
  ];

  $form['add'] = [
    '#type' => 'link',
    '#title' => t('Add new replacement'),
    '#href' => 'admin/config/acuity-utils/acuity_text/custom/add',
    '#attributes' => ['class' => ['button', 'button-action']],
  ];

  return $form;
}

/**
 * Form for adding or editing a custom replacement.
 */
function acuity_text_formatter_edit_form($form, &$form_state, $id = NULL) {
  $config = config('acuity_text_formatter.settings');
  $replacements = $config->get('custom_replacements') ?: [];
  $data = ($id && isset($replacements[$id])) ? $replacements[$id] : [];

  $form['id'] = [
    '#type' => 'value',
    '#value' => $id,
  ];

  $form['search'] = [
    '#type' => 'textfield',
    '#title' => t('Search for'),
    '#description' => t('The standalone word or acronym to match. The match is case-insensitive and whole-word only.'),
    '#default_value' => $data['search'] ?? '',
    '#maxlength' => 128,
    '#required' => TRUE,
  ];

  $form['replace'] = [
    '#type' => 'textfield',
    '#title' => t('Replace with'),
    '#default_value' => $data['replace'] ?? '',
    '#maxlength' => 128,
    '#required' => TRUE,
  ];

  $form['actions'] = ['#type' => 'actions'];

  $form['actions']['submit'] = [
    '#type' => 'submit',
    '#value' => t('Save'),
    '#button_type' => 'submit',
    '#attributes' => ['class' => ['button-primary']],
  ];

  $form['actions']['cancel'] = [
    '#type' => 'link',
    '#title' => t('Cancel'),
    '#href' => 'admin/config/acuity-utils/acuity_text/custom',
    '#attributes' => ['class' => ['button']],
  ];

  return $form;
}

/**
 * Validation handler for custom replacement form.
 */
function acuity_text_formatter_edit_form_validate($form, &$form_state) {
  $search = trim($form_state['values']['search'] ?? '');
  $replace = trim($form_state['values']['replace'] ?? '');

  if ($search === '') {
    form_set_error('search', t('Search text cannot be empty.'));
  }

  if (backdrop_strlen($search) > 128) {
    form_set_error('search', t('Search text must be 128 characters or fewer.'));
  }

  if (backdrop_strlen($replace) > 128) {
    form_set_error('replace', t('Replacement text must be 128 characters or fewer.'));
  }
}

/**
 * Submit handler for custom replacement form.
 */
function acuity_text_formatter_edit_form_submit($form, &$form_state) {
  $config = config('acuity_text_formatter.settings');
  $replacements = $config->get('custom_replacements') ?: [];

  $id = $form_state['values']['id'];
  if (empty($id)) {
    $id = uniqid('', TRUE);
  }

  $replacements[$id] = [
    'search' => trim($form_state['values']['search']),
    'replace' => trim($form_state['values']['replace']),
  ];

  $config->set('custom_replacements', $replacements)->save();

  backdrop_set_message(t('Custom replacement saved.'));
  $form_state['redirect'] = 'admin/config/acuity-utils/acuity_text/custom';
}

/**
 * Confirmation form for deleting a custom replacement.
 */
function acuity_text_formatter_delete_form($form, &$form_state, $id) {
  $config = config('acuity_text_formatter.settings');
  $replacements = $config->get('custom_replacements') ?: [];

  if (!isset($replacements[$id])) {
    backdrop_set_message(t('Replacement not found.'), 'error');
    backdrop_goto('admin/config/acuity-utils/acuity_text/custom');
  }

  $form['id'] = [
    '#type' => 'value',
    '#value' => $id,
  ];

  return confirm_form(
    $form,
    t('Delete replacement "%search"?', ['%search' => $replacements[$id]['search']]),
    'admin/config/acuity-utils/acuity_text/custom',
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Submit handler for deleting a custom replacement.
 */
function acuity_text_formatter_delete_form_submit($form, &$form_state) {
  $id = $form_state['values']['id'];

  $config = config('acuity_text_formatter.settings');
  $replacements = $config->get('custom_replacements') ?: [];

  if (isset($replacements[$id])) {
    unset($replacements[$id]);
    $config->set('custom_replacements', $replacements)->save();
    backdrop_set_message(t('Replacement deleted.'));
  }

  $form_state['redirect'] = 'admin/config/acuity-utils/acuity_text/custom';
}

/**
 * Title assignment listing form.
 */
function acuity_text_formatter_titles_form($form, &$form_state) {
  $title_config = config('acuity_text_formatter.titles');
  $assignments = $title_config->get('assignments') ?: [];

  $form['description'] = [
    '#markup' => '<p>' . t('Configure text formatting for content type titles.') . '</p>',
  ];

  $header = [
    t('Edit'),
    t('Content type'),
    t('Formatting rules'),
    t('Remove'),
  ];

  $rows = [];

  foreach ($assignments as $type => $settings) {
    $rows[] = [
      l(t('Edit'), "admin/config/acuity-utils/acuity_text/titles/edit/$type"),
      check_plain(node_type_get_name($type)),
      check_plain(_acuity_text_formatter_rule_summary($settings)),
      l(t('Remove'), "admin/config/acuity-utils/acuity_text/titles/remove/$type", [
        'attributes' => ['class' => ['acuity-remove-link']],
      ]),
    ];
  }

  $form['table'] = [
    '#markup' => theme('table', [
      'header' => $header,
      'rows' => $rows,
      'empty' => t('No title assignments yet.'),
    ]),
  ];

  $form['add'] = [
    '#type' => 'link',
    '#title' => t('Assign formatting to title'),
    '#href' => 'admin/config/acuity-utils/acuity_text/titles/add',
    '#attributes' => ['class' => ['button', 'button-primary']],
  ];

  return $form;
}

/**
 * Form to assign title formatting to a content type.
 */
function acuity_text_formatter_assign_title_form($form, &$form_state) {
  $title_config = config('acuity_text_formatter.titles');
  $assignments = $title_config->get('assignments') ?: [];

  $all_types = node_type_get_types();
  $available_types = [];

  foreach ($all_types as $type => $info) {
    if (!isset($assignments[$type])) {
      $available_types[$type] = $info->name;
    }
  }

  if (empty($available_types)) {
    backdrop_set_message(t('All content types are already assigned.'), 'warning');
    backdrop_goto('admin/config/acuity-utils/acuity_text/titles');
  }

  $form['content_type'] = [
    '#type' => 'select',
    '#title' => t('Content type'),
    '#options' => $available_types,
    '#required' => TRUE,
  ];

  $form['trim'] = [
    '#type' => 'checkbox',
    '#title' => t('Trim leading/trailing spaces'),
    '#default_value' => 0,
  ];

  $form['collapse_spaces'] = [
    '#type' => 'checkbox',
    '#title' => t('Collapse multiple spaces'),
    '#description' => t('Converts repeated spaces into a single space, e.g. "John   Smith" becomes "John Smith".'),
    '#default_value' => 1,
  ];

  $form['case_transform'] = [
    '#type' => 'select',
    '#title' => t('Case Transformation'),
    '#options' => _acuity_text_formatter_case_options(),
    '#default_value' => 'none',
  ];

  $form['actions'] = ['#type' => 'actions'];

  $form['actions']['submit'] = [
    '#type' => 'submit',
    '#value' => t('Save assignment'),
    '#button_type' => 'submit',
    '#attributes' => ['class' => ['button-primary']],
  ];

  $form['actions']['cancel'] = [
    '#type' => 'link',
    '#title' => t('Cancel'),
    '#href' => 'admin/config/acuity-utils/acuity_text/titles',
    '#attributes' => ['class' => ['button']],
  ];

  return $form;
}

/**
 * Submit handler for assigning title formatting.
 */
function acuity_text_formatter_assign_title_form_submit($form, &$form_state) {
  $content_type = $form_state['values']['content_type'];

  $title_config = config('acuity_text_formatter.titles');
  $assignments = $title_config->get('assignments') ?: [];

  $assignments[$content_type] = [
    'trim' => !empty($form_state['values']['trim']),
    'collapse_spaces' => !empty($form_state['values']['collapse_spaces']),
    'case_transform' => $form_state['values']['case_transform'] ?? 'none',
  ];

  $title_config->set('assignments', $assignments)->save();

  backdrop_set_message(t('Title formatting assigned to content type %type.', [
    '%type' => node_type_get_name($content_type),
  ]));

  $form_state['redirect'] = 'admin/config/acuity-utils/acuity_text/titles';
}

/**
 * Form to edit title formatting.
 */
function acuity_text_formatter_edit_title_form($form, &$form_state, $type) {
  $title_config = config('acuity_text_formatter.titles');
  $assignments = $title_config->get('assignments') ?: [];

  if (!isset($assignments[$type])) {
    backdrop_set_message(t('Title assignment not found.'), 'error');
    backdrop_goto('admin/config/acuity-utils/acuity_text/titles');
  }

  $settings = $assignments[$type];

  $form['type'] = [
    '#type' => 'value',
    '#value' => $type,
  ];

  $form['title_display'] = [
    '#markup' => '<h2>' . t('Edit Title Formatting: @type', [
      '@type' => node_type_get_name($type),
    ]) . '</h2>',
  ];

  $form['trim'] = [
    '#type' => 'checkbox',
    '#title' => t('Trim leading/trailing spaces'),
    '#default_value' => $settings['trim'] ?? 0,
  ];

  $form['collapse_spaces'] = [
    '#type' => 'checkbox',
    '#title' => t('Collapse multiple spaces'),
    '#description' => t('Converts repeated spaces into a single space, e.g. "John   Smith" becomes "John Smith".'),
    '#default_value' => $settings['collapse_spaces'] ?? 1,
  ];

  $form['case_transform'] = [
    '#type' => 'select',
    '#title' => t('Case Transformation'),
    '#options' => _acuity_text_formatter_case_options(),
    '#default_value' => $settings['case_transform'] ?? 'none',
  ];

  $form['actions'] = ['#type' => 'actions'];

  $form['actions']['submit'] = [
    '#type' => 'submit',
    '#value' => t('Save changes'),
    '#button_type' => 'submit',
    '#attributes' => ['class' => ['button-primary']],
  ];

  $form['actions']['cancel'] = [
    '#type' => 'link',
    '#title' => t('Cancel'),
    '#href' => 'admin/config/acuity-utils/acuity_text/titles',
    '#attributes' => ['class' => ['button']],
  ];

  return $form;
}

/**
 * Submit handler for editing title formatting.
 */
function acuity_text_formatter_edit_title_form_submit($form, &$form_state) {
  $type = $form_state['values']['type'];

  $title_config = config('acuity_text_formatter.titles');
  $assignments = $title_config->get('assignments') ?: [];

  $assignments[$type] = [
    'trim' => !empty($form_state['values']['trim']),
    'collapse_spaces' => !empty($form_state['values']['collapse_spaces']),
    'case_transform' => $form_state['values']['case_transform'] ?? 'none',
  ];

  $title_config->set('assignments', $assignments)->save();

  backdrop_set_message(t('Title formatting updated for %type.', [
    '%type' => node_type_get_name($type),
  ]));

  $form_state['redirect'] = 'admin/config/acuity-utils/acuity_text/titles';
}

/**
 * Form to remove title formatting.
 */
function acuity_text_formatter_remove_title_form($form, &$form_state, $type) {
  $title_config = config('acuity_text_formatter.titles');
  $assignments = $title_config->get('assignments') ?: [];

  if (!isset($assignments[$type])) {
    backdrop_set_message(t('Title assignment not found.'), 'error');
    backdrop_goto('admin/config/acuity-utils/acuity_text/titles');
  }

  $form['type'] = [
    '#type' => 'value',
    '#value' => $type,
  ];

  return confirm_form(
    $form,
    t('Are you sure you want to remove title formatting for %type?', [
      '%type' => node_type_get_name($type),
    ]),
    'admin/config/acuity-utils/acuity_text/titles',
    t('This action cannot be undone.'),
    t('Remove'),
    t('Cancel')
  );
}

/**
 * Submit handler for removing title formatting.
 */
function acuity_text_formatter_remove_title_form_submit($form, &$form_state) {
  $type = $form_state['values']['type'];

  $title_config = config('acuity_text_formatter.titles');
  $assignments = $title_config->get('assignments') ?: [];

  if (isset($assignments[$type])) {
    unset($assignments[$type]);
    $title_config->set('assignments', $assignments)->save();

    backdrop_set_message(t('Title formatting removed for %type.', [
      '%type' => node_type_get_name($type),
    ]));
  }

  $form_state['redirect'] = 'admin/config/acuity-utils/acuity_text/titles';
}

/**
 * User account name formatting form.
 */
function acuity_text_formatter_users_form($form, &$form_state) {
  $user_config = config('acuity_text_formatter.users');
  $settings = $user_config->get('name') ?: [];

  $form['description'] = [
    '#markup' => '<p>' . t('Configure text formatting for the core user account name when a user account is saved.') . '</p>',
  ];

  $form['current'] = [
    '#markup' => '<p><strong>' . t('Current rules:') . '</strong> ' . check_plain(_acuity_text_formatter_rule_summary($settings)) . '</p>',
  ];

  $form['name'] = [
    '#type' => 'fieldset',
    '#title' => t('User account name formatting'),
  ];

  $form['name']['trim'] = [
    '#type' => 'checkbox',
    '#title' => t('Trim leading/trailing spaces'),
    '#default_value' => $settings['trim'] ?? 0,
  ];

  $form['name']['collapse_spaces'] = [
    '#type' => 'checkbox',
    '#title' => t('Collapse multiple spaces'),
    '#description' => t('Converts repeated spaces into a single space, e.g. "John   Smith" becomes "John Smith".'),
    '#default_value' => $settings['collapse_spaces'] ?? 1,
  ];

  $form['name']['case_transform'] = [
    '#type' => 'select',
    '#title' => t('Case Transformation'),
    '#options' => _acuity_text_formatter_case_options(),
    '#default_value' => $settings['case_transform'] ?? 'none',
  ];

  $form['actions'] = ['#type' => 'actions'];

  $form['actions']['submit'] = [
    '#type' => 'submit',
    '#value' => t('Save user settings'),
    '#button_type' => 'submit',
    '#attributes' => ['class' => ['button-primary']],
  ];

  return $form;
}

/**
 * Submit handler for user account name formatting form.
 */
function acuity_text_formatter_users_form_submit($form, &$form_state) {
  $user_config = config('acuity_text_formatter.users');

  $user_config->set('name', [
    'trim' => !empty($form_state['values']['trim']),
    'collapse_spaces' => !empty($form_state['values']['collapse_spaces']),
    'case_transform' => $form_state['values']['case_transform'] ?? 'none',
  ])->save();

  backdrop_set_message(t('User account name formatting updated.'));
  $form_state['redirect'] = 'admin/config/acuity-utils/acuity_text/users';
}