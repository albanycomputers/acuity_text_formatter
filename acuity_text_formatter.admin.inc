<?php
/**
 * Main Admin settings form.
 */
function acuity_text_formatter_admin_form($form, &$form_state) {
  $config = config('acuity_text_formatter.settings');

  // --- 1. Global Punctuation Settings ---
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

  $form['actions'] = ['#type' => 'actions'];
  $form['actions']['submit'] = [
    '#type' => 'submit',
    '#value' => t('Save global settings'),
  ];
  $form['#submit'][] = 'system_settings_form_submit';


  // --- 2. Custom Replacements Section ---
  $form['replacements_wrapper'] = [
    '#type' => 'container',
    '#attributes' => ['class' => ['acuity-admin-section', 'custom-replacements']],
    '#weight' => 10,
  ];

  $form['replacements_wrapper']['header'] = [
    '#markup' => '<h3>' . t('Custom Replacements') . '</h3>' .
                 '<p>' . t('Define specific words or acronyms to force-format (e.g., "usa" -> "USA"). These apply to both <strong>CamelCase</strong> and <strong>Intercapped</strong> modes.') . '</p>',
  ];

  $replacements = $config->get('custom_replacements');
  if (empty($replacements)) {
    $defaults = [
      uniqid() => ['search' => 'uk', 'replace' => 'UK'],
      uniqid() => ['search' => 'usa', 'replace' => 'USA'],
      uniqid() => ['search' => 'llc', 'replace' => 'LLC'],
    ];
    $config->set('custom_replacements', $defaults);
    $config->save();
    $replacements = $defaults;
  }

  $header = [t('Edit'), t('Search For (Input)'), t('Replace With (Output)'), t('Delete')];
  $rows = [];
  if (!empty($replacements)) {
    foreach ($replacements as $id => $data) {
      $rows[] = [
        l(t('Edit'), "admin/config/acuity-utils/acuity_text/edit/$id"),
        check_plain($data['search']),
        check_plain($data['replace']),
        l(t('Delete'), "admin/config/acuity-utils/acuity_text/delete/$id", ['attributes' => ['class' => ['abms-delete-link']]]),
      ];
    }
  }

  $form['replacements_wrapper']['table'] = ['#markup' => theme('table', ['header' => $header, 'rows' => $rows, 'empty' => t('No replacements defined.')])];
  $form['replacements_wrapper']['add'] = [
    '#type' => 'link',
    '#title' => t('Add new replacement'),
    '#href' => 'admin/config/acuity-utils/acuity_text/add',
    '#attributes' => ['class' => ['button', 'button-action']],
  ];


  // --- 3. Title Field Assignments Section ---
  $form['title_assignments_wrapper'] = [
    '#type' => 'container',
    '#attributes' => ['class' => ['acuity-admin-section', 'acuity-title-assignments']],
    '#weight' => 20,
  ];

  $form['title_assignments_wrapper']['description'] = [
    '#markup' => '<h3>' . t('Title Field Assignments') . '</h3>' .
                 '<p>' . t('Configure text formatting for content type titles.') . '</p>',
  ];

  // Load from separate config file
  $title_config = config('acuity_text_formatter.titles');
  $assignments = $title_config->get('assignments');

  $header = [t('Edit'), t('Content Type'), t('Formatting Rules'), t('Remove')];
  $rows = [];

  if (!empty($assignments)) {
    foreach ($assignments as $type => $settings) {
      $rules = [];
      if (!empty($settings['trim'])) {
        $rules[] = t('Trim');
      }
      if (!empty($settings['case_transform']) && $settings['case_transform'] !== 'none') {
        $labels = [
          'lowercase' => t('Lowercase'),
          'uppercase' => t('Uppercase'),
          'camelcase' => t('CamelCase'),
          'intercapped' => t('Intercapped'),
        ];
        $rules[] = isset($labels[$settings['case_transform']]) ? $labels[$settings['case_transform']] : $settings['case_transform'];
      }

      $rows[] = [
        l(t('Edit'), "admin/config/acuity-utils/acuity_text/edit-title/$type", ['attributes' => ['title' => t('Edit title assignment')]]),
        check_plain(node_type_get_name($type)),
        implode(', ', $rules),
        l(t('Remove'), "admin/config/acuity-utils/acuity_text/remove-title/$type", ['attributes' => ['class' => ['abms-remove-link']]]),
      ];
    }

    $form['title_assignments_wrapper']['table'] = [
      '#markup' => theme('table', ['header' => $header, 'rows' => $rows]),
    ];
  } else {
    $form['title_assignments_wrapper']['empty'] = [
      '#markup' => '<p><em>' . t('No title assignments yet.') . '</em></p>',
    ];
  }

  $form['title_assignments_wrapper']['add'] = [
    '#type' => 'link',
    '#title' => t('Assign formatting to title'),
    '#href' => 'admin/config/acuity-utils/acuity_text/assign-title',
    '#attributes' => ['class' => ['button', 'button-primary']],
  ];

  return $form;
}

// --- Custom Replacements Forms ---

function acuity_text_formatter_edit_form($form, &$form_state, $id = NULL) {
  $config = config('acuity_text_formatter.settings');
  $replacements = $config->get('custom_replacements') ?: [];
  $data = ($id && isset($replacements[$id])) ? $replacements[$id] : [];

  $form['id'] = ['#type' => 'value', '#value' => $id];
  $form['search'] = [
    '#type' => 'textfield',
    '#title' => t('Search for'),
    '#default_value' => isset($data['search']) ? $data['search'] : '',
    '#required' => TRUE,
  ];
  $form['replace'] = [
    '#type' => 'textfield',
    '#title' => t('Replace with'),
    '#default_value' => isset($data['replace']) ? $data['replace'] : '',
    '#required' => TRUE,
  ];
  $form['actions'] = ['#type' => 'actions'];
  $form['actions']['submit'] = ['#type' => 'submit', '#value' => t('Save')];
  $form['actions']['cancel'] = ['#type' => 'link', '#title' => t('Cancel'), '#href' => 'admin/config/acuity-utils/acuity_text', '#attributes' => ['class' => ['button', 'button-secondary']]];

  return $form;
}

function acuity_text_formatter_edit_form_submit($form, &$form_state) {
  $config = config('acuity_text_formatter.settings');
  $replacements = $config->get('custom_replacements') ?: [];
  $id = $form_state['values']['id'];
  if (empty($id)) { $id = uniqid(); }

  $replacements[$id] = [
    'search' => trim($form_state['values']['search']),
    'replace' => trim($form_state['values']['replace']),
  ];
  $config->set('custom_replacements', $replacements)->save();
  backdrop_set_message(t('Custom replacement saved.'));
  $form_state['redirect'] = 'admin/config/acuity-utils/acuity_text';
}

function acuity_text_formatter_delete_form($form, &$form_state, $id) {
  $config = config('acuity_text_formatter.settings');
  $replacements = $config->get('custom_replacements');
  if (!isset($replacements[$id])) {
    backdrop_set_message(t('Replacement not found.'), 'error');
    backdrop_goto('admin/config/acuity-utils/acuity_text');
  }
  $form['id'] = ['#type' => 'value', '#value' => $id];
  return confirm_form($form, t('Delete replacement "%search"?', ['%search' => $replacements[$id]['search']]), 'admin/config/acuity-utils/acuity_text', t('This action cannot be undone.'), t('Delete'), t('Cancel'));
}

function acuity_text_formatter_delete_form_submit($form, &$form_state) {
  $id = $form_state['values']['id'];
  $config = config('acuity_text_formatter.settings');
  $replacements = $config->get('custom_replacements');
  if (isset($replacements[$id])) {
    unset($replacements[$id]);
    $config->set('custom_replacements', $replacements)->save();
    backdrop_set_message(t('Replacement deleted.'));
  }
  $form_state['redirect'] = 'admin/config/acuity-utils/acuity_text';
}


// --- Title Assignment Forms ---

/**
 * Form to Assign a new Title formatting rule.
 */
function acuity_text_formatter_assign_title_form($form, &$form_state) {
  // Load existing assignments
  $title_config = config('acuity_text_formatter.titles');
  $assignments = $title_config->get('assignments') ?: [];

  // Calculate available content types
  $all_types = node_type_get_types();
  $available_types = [];
  foreach ($all_types as $type => $info) {
    if (!isset($assignments[$type])) {
      $available_types[$type] = $info->name;
    }
  }

  if (empty($available_types)) {
    backdrop_set_message(t('All content types are already assigned.'), 'warning');
    backdrop_goto('admin/config/acuity-utils/acuity_text');
  }

  $form['content_type'] = [
    '#type' => 'select',
    '#title' => t('Content type'),
    '#options' => $available_types,
    '#required' => TRUE,
  ];

  // Formatting Options (Matching Widget UI)
  $form['trim'] = [
    '#type' => 'checkbox',
    '#title' => t('Trim leading/trailing spaces'),
    '#default_value' => 0,
  ];

  $form['case_transform'] = [
    '#type' => 'select',
    '#title' => t('Case Transformation'),
    '#options' => [
      'none' => t('- None -'),
      'lowercase' => t('Convert to lowercase'),
      'uppercase' => t('Convert to uppercase'),
      'camelcase' => t('CamelCase'),
      'intercapped' => t('Intercapped (e.g., McCallister, O\'Connor)'),
    ],
    '#default_value' => 'none',
  ];

  $form['actions'] = ['#type' => 'actions'];
  $form['actions']['submit'] = ['#type' => 'submit', '#value' => t('Save assignment'), '#button_type' => 'primary'];
  $form['actions']['cancel'] = ['#type' => 'link', '#title' => t('Cancel'), '#href' => 'admin/config/acuity-utils/acuity_text', '#attributes' => ['class' => ['button', 'button-secondary']]];

  return $form;
}

function acuity_text_formatter_assign_title_form_submit($form, &$form_state) {
  $content_type = $form_state['values']['content_type'];

  $title_config = config('acuity_text_formatter.titles');
  $assignments = $title_config->get('assignments') ?: [];

  $assignments[$content_type] = [
    'trim' => $form_state['values']['trim'],
    'case_transform' => $form_state['values']['case_transform'],
  ];

  $title_config->set('assignments', $assignments)->save();

  backdrop_set_message(t('Title formatting assigned to content type %type.', ['%type' => node_type_get_name($content_type)]));
  $form_state['redirect'] = 'admin/config/acuity-utils/acuity_text';
}

/**
 * Form to Edit an existing Title assignment.
 */
function acuity_text_formatter_edit_title_form($form, &$form_state, $type) {
  $title_config = config('acuity_text_formatter.titles');
  $assignments = $title_config->get('assignments') ?: [];
  $settings = isset($assignments[$type]) ? $assignments[$type] : [];

  $form['type'] = [
    '#type' => 'value',
    '#value' => $type,
  ];

  $form['title_display'] = [
    '#markup' => '<h2>' . t('Edit Title Formatting: @type', ['@type' => node_type_get_name($type)]) . '</h2>',
  ];

  $form['trim'] = [
    '#type' => 'checkbox',
    '#title' => t('Trim leading/trailing spaces'),
    '#default_value' => isset($settings['trim']) ? $settings['trim'] : 0,
  ];

  $form['case_transform'] = [
    '#type' => 'select',
    '#title' => t('Case Transformation'),
    '#options' => [
      'none' => t('- None -'),
      'lowercase' => t('Convert to lowercase'),
      'uppercase' => t('Convert to uppercase'),
      'camelcase' => t('CamelCase'),
      'intercapped' => t('Intercapped (e.g., McCallister, O\'Connor)'),
    ],
    '#default_value' => isset($settings['case_transform']) ? $settings['case_transform'] : 'none',
  ];

  $form['actions'] = ['#type' => 'actions'];
  $form['actions']['submit'] = ['#type' => 'submit', '#value' => t('Save changes')];
  $form['actions']['cancel'] = ['#type' => 'link', '#title' => t('Cancel'), '#href' => 'admin/config/acuity-utils/acuity_text', '#attributes' => ['class' => ['button', 'button-secondary']]];

  return $form;
}

function acuity_text_formatter_edit_title_form_submit($form, &$form_state) {
  $type = $form_state['values']['type'];

  $title_config = config('acuity_text_formatter.titles');
  $assignments = $title_config->get('assignments') ?: [];

  $assignments[$type] = [
    'trim' => $form_state['values']['trim'],
    'case_transform' => $form_state['values']['case_transform'],
  ];

  $title_config->set('assignments', $assignments)->save();

  backdrop_set_message(t('Title formatting updated for %type.', ['%type' => node_type_get_name($type)]));
  $form_state['redirect'] = 'admin/config/acuity-utils/acuity_text';
}

/**
 * Form to Remove a Title assignment.
 */
function acuity_text_formatter_remove_title_form($form, &$form_state, $type) {
  $form['type'] = ['#type' => 'value', '#value' => $type];

  return confirm_form(
    $form,
    t('Are you sure you want to remove title formatting for %type?', ['%type' => node_type_get_name($type)]),
    'admin/config/acuity-utils/acuity_text',
    t('This action cannot be undone.'),
    t('Remove'),
    t('Cancel')
  );
}

function acuity_text_formatter_remove_title_form_submit($form, &$form_state) {
  $type = $form_state['values']['type'];
  $title_config = config('acuity_text_formatter.titles');
  $assignments = $title_config->get('assignments') ?: [];

  if (isset($assignments[$type])) {
    unset($assignments[$type]);
    $title_config->set('assignments', $assignments)->save();
    backdrop_set_message(t('Title formatting removed for %type.', ['%type' => node_type_get_name($type)]));
  }

  $form_state['redirect'] = 'admin/config/acuity-utils/acuity_text';
}