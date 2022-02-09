Entity Registration
===================

An entity-based registration system for Backdrop CMS.

## Usage / Features

1. Manage registrations for any enabled entity.
2. Per entity registration settings.
3. Broadcast emails to all event registrants.
4. Associate any field types to a registration to collect the information needed
   for your event.
5. Send reminders on a given date.

## Integrations for more functionality

### [Fields](http://api.drupal.org/api/drupal/modules--field--field.module/group/field/7)

This is where things get interesting. You can add any Drupal field to customize
your registrations. The fields widgets will automatically appear on the register
form and will be available from a registration detail page.

### [Views](http://drupal.org/project/views)

Not happy with the default tabular list of registrations? No problem,
registrations and their fields are all Views friendly. You can override the
default event registrations list, create additional ones, etc.

### [Rules](http://drupal.org/project/rules)

Rules is a great companion for Registration to send confirmation emails, event
notifications, etc.

### Registrants via [Field Collection](http://drupal.org/project/field_collection)

Attaching a field collection field to a registration allows you to collect
granular information for multiple registrants for a single registration. Here's
how it works.

1. Download and enable Field Collection.
2. Add a field collection field to your registration entity.
3. Add any fields that you want to collect to the field collection entity and
   configure widget and display settings. You might also want to consider field
   collection table to create tabular lists of registrants.

That's it. Now, when a registration is added, users can complete one or more
field collections for each registrant.


Requirements
------------

This module requires that the following modules are also enabled:

 * [Entity Plus](https://github.com/backdrop-contrib/entity_plus)

Installation
------------

- Install this module using the official Backdrop CMS instructions at
  https://backdropcms.org/guide/modules.

- Visit the configuration page under Administration > Structure > Registration >
  Types (/admin/structure/registration/registration_types) and create at least
  one registration type, similar to how you might create a content type.

- Add a registration field to an entity where you would like registration. For
  example, adda Registration field to an event node type. (Note the display
  options: default, link to registration form, and embeded registration form.)
  - Select the registration bundle you created above.

- Registrations are now enabled. Create or edit an entity.
  - Use the "Manage Registrations" tab to see who's registered, and adjust the
  registration settings on the "Settings" sub-tab.


  ## Settings

  1. Enable:
     Turn registrations on / off for a given node.
  2. Capacity:
     The maximum number of registrants for an entity. Leave at 0 for no limit.
  3. Allow Multiple:
     If selected, users can register for more than one slot for this event.
  4. Send a reminder.
     Checking this exposes reminder date and message template fields.

Documentation
-------------

Additional documentation is located in the Wiki:
https://github.com/backdrop-contrib/registration/wiki/Documentation.

Issues
------

Bugs and Feature requests should be reported in the Issue Queue:
https://github.com/backdrop-contrib/registration/issues.

Current Maintainers
-------------------

- [Joseph Flatt](https://github.com/hosef).
- [Andy Shillingford](https://github.com/docwilmot).
- Seeking additional maintainers.

Credits
-------

- Ported to Backdrop CMS by [Joseph Flatt](https://github.com/hosef).
- Improved for Backdrop CMS by [Andy Shillingford](https://github.com/docwilmot).
- Originally written for Drupal by [Lev Tsypin](https://www.drupal.org/u/levelos).
- Maintained for Drupal by [many generous contributors](https://www.drupal.org/node/1284480/committers)

License
-------

This project is GPL v2 software.
See the LICENSE.txt file in this directory for complete text.

