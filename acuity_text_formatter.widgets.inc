<?php

/**
 * Implements hook_field_widget_info().
 */
function acuity_text_formatter_field_widget_info() {
  return [
    'acuity_text_formatter_widget' => [
      'label' => t('Acuity Text Formatter'),
      'field types' => ['text', 'text_long', 'text_with_summary'],
      'settings' => [
        // Updated defaults to match the new nested structure
        'acuity_formatter_settings' => [
          'trim' => 0,
          'case_transform' => 'none',
        ],
      ],
      'module' => 'acuity_text_formatter',
    ],
  ];
}

/**
 * Implements hook_field_widget_settings_form().
 */
function acuity_text_formatter_field_widget_settings_form($field, $instance) {
  $settings = $instance['widget']['settings'];

  // Get values from the nested array, or fall back to defaults
  $defaults = isset($settings['acuity_formatter_settings']) ? $settings['acuity_formatter_settings'] : [];

  // 1. Container Fieldset
  $form['acuity_formatter_settings'] = [
    '#type' => 'fieldset',
    '#title' => t('Text Formatting'),
    '#description' => t('Configure how text is automatically formatted when saved.'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  ];

  // 2. Trim
  $form['acuity_formatter_settings']['trim'] = [
    '#type' => 'checkbox',
    '#title' => t('Trim leading/trailing spaces'),
    '#default_value' => isset($defaults['trim']) ? $defaults['trim'] : 0,
  ];

  // 3. Case Modes
  $form['acuity_formatter_settings']['case_transform'] = [
    '#type' => 'select',
    '#title' => t('Case Transformation'),
    '#description' => t('Select a case transformation to apply on save.'),
    '#options' => [
      'none' => t('- None -'),
      'lowercase' => t('Convert to lowercase'),
      'uppercase' => t('Convert to uppercase'),
      'camelcase' => t('Capitalize Each Word (CamelCase)'),
      'intercapped' => t('Intercapped (e.g., McCallister, O\'Connor)'),
    ],
    '#default_value' => isset($defaults['case_transform']) ? $defaults['case_transform'] : 'none',
  ];

  return $form;
}

/**
 * Implements hook_field_widget_settings_summary().
 */
function acuity_text_formatter_field_widget_settings_summary($field, $instance) {
  $settings = $instance['widget']['settings'];
  // Extract our specific settings from the wrapper
  $format_settings = isset($settings['acuity_formatter_settings']) ? $settings['acuity_formatter_settings'] : [];

  $summary = [];

  // Report Trim status
  if (!empty($format_settings['trim'])) {
    $summary[] = t('Trim: Yes');
  }

  // Report Case Transform
  if (!empty($format_settings['case_transform']) && $format_settings['case_transform'] !== 'none') {
    $labels = [
      'lowercase' => t('Lowercase'),
      'uppercase' => t('Uppercase'),
      'camelcase' => t('CamelCase'),
      'intercapped' => t('Intercapped'),
    ];
    // Look up the readable label or fall back to the key
    $label = isset($labels[$format_settings['case_transform']]) ? $labels[$format_settings['case_transform']] : $format_settings['case_transform'];
    $summary[] = t('Case: @mode', ['@mode' => $label]);
  }

  if (empty($summary)) {
    $summary[] = t('No formatting enabled');
  }

  return $summary;
}

/**
 * Implements hook_field_widget_form().
 */
function acuity_text_formatter_field_widget_form($form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {
  $value = isset($items[$delta]['value']) ? $items[$delta]['value'] : '';

  $element['value'] = [
    '#type' => 'textfield',
    '#default_value' => $value,
    '#size' => 60,
    '#maxlength' => !empty($field['settings']['max_length']) ? $field['settings']['max_length'] : 255,
  ];

  return $element;
}