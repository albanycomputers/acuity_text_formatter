# Text Formatter (Acuity Utils)
**Text Formatter (Acuity Utils)** provides a custom field widget for Backdrop CMS that automatically formats text fields when content is saved. It is designed to ensure data consistency for names, addresses, and postcodes by correcting capitalization, spacing, and punctuation errors during data entry.

## Features

* **Trimming:** Automatically removes leading and trailing whitespace.
* **Case Transformations:**
    * **Lowercase:** Converts text to lowercase.
    * **Uppercase:** Converts text to uppercase (useful for Postcodes/Zip codes).
    * **CamelCase:** Standard "Title Case" that respects global punctuation settings and Custom Replacements.
    * **Intercapped:** Intelligent name formatting that runs all CamelCase logic *plus* strict "Mc/Mac" name correction.
* **Smart Punctuation:** Capitalizes text automatically after:
    * Hyphens (`Jean-Luc`)
    * Apostrophes (`O'Connor`)
    * Brackets (`(Text inside)`)
    * Periods (`Sentence one. Sentence two.`)
    * New Lines (`Address Line 1` \n `Address Line 2`)
* **Custom Replacements:** Define specific overrides for acronyms or brand names (e.g., "usa" -> "USA", "iphone" -> "iPhone"). *Applies to both CamelCase and Intercapped modes.*
* **Safety Lock:** Automatically forces the field to **Plain Text** mode to prevent accidental corruption of HTML tags by the case formatter.

### Requirements:

- Backdrop CMS 1.x
- PHP 8+ (While the code may technically function with PHP 7.4 at this time, we strictly require PHP 8.0+ and will not address issues related to older PHP versions.)

## Installation:

Install this module using the official Backdrop CMS instructions at https://docs.backdropcms.org/documentation/extend-with-modules

Enable the module.

## Configuration

There are two parts to configuring this module: Global Settings and Field Widget Settings.

### 1. Global Settings
Go to **Configuration > Acuity Utils > Text Formatter Settings** (`admin/config/acuity-utils/acuity_text`).

* **Global Punctuation:** Configure capitalization rules for hyphens, apostrophes, brackets, periods, and new lines.
* **Custom Replacements:** Add, edit, or delete specific words that should always be formatted a certain way (e.g., "UK", "LLC", "USA"). *These apply to both **CamelCase** and **Intercapped** modes.*

### 2. Field Configuration
To apply formatting to a specific field (e.g., a "First Name" or "Address" field):

1.  Navigate to the **Manage Fields** tab of your Content Type (e.g., `admin/structure/types/manage/page/fields`).
2.  Locate the text field you wish to format (supports Text, Long Text, and Text with Summary).
3.  Change the **Widget Type** to **Acuity Text Formatter**.
4.  Click the **cog wheel (configure)** icon next to the widget settings.
5.  Expand the **Text Formatting** fieldset and select your options:
    * **Trim leading/trailing spaces:** (Recommended)
    * **Case Transformation:** Select *Lowercase*, *Uppercase*, *CamelCase*, or *Intercapped*.
6.  Save the settings.

## Usage Notes

### HTML Safety (Plain Text Enforcement)
When the **Acuity Text Formatter** widget is active on a field, the module automatically disables WYSIWYG editors and forces the **Text Processing** setting to **Plain text**.

This is a safety feature to ensure that case-conversion logic (like `ucwords`) does not break HTML tags (e.g., converting `<div id="main">` to `<Div Id="Main">`), which would break your site's layout.

### Choosing the Right Mode
* **CamelCase:** Best for standard titles, simple sentences, or short descriptions. It respects your global punctuation settings and applies Custom Replacements.
* **Intercapped:** Best for **Names** and **Addresses**. It runs the CamelCase logic *plus* the intelligent "Mc/Mac" name correction.


## Issues:

Bugs and Feature requests should be reported in the Issue Queue: https://github.com/albanycomputers/acuity_text_formatter/issues

## About the "Acuity" Name
"Acuity" is the name used to organise a collection of modules and utilities. These tools are grouped under a unified name to streamline installation and make it easier for site maintainers to identify these suite of solutions.

Technically, the namespaces `acuity_` and `abms_` were chosen to enhance code readability and keep function names simple and easy to type.

While part of this larger Acuity family, this module is a standalone utility.

## Current Maintainer(s):
- Steve Moorhouse (albanycomputers) (https://github.com/albanycomputers)
- Seeking additional maintainers and contributors.

## Credits:
- Steve Moorhouse - Zulip (DrAlbany)
- Google Gemini 3.0 assisted with the coding of this module.

## Sponsorship:
- Albany Computer Services (https://www.albany-computers.co.uk)
- Albany Web Design (https://www.albanywebdesign.co.uk)
- Albany Hosting (https://www.albany-hosting.co.uk)

## License
This project is GPL v2 software. See the LICENSE.txt file in this directory for complete text.