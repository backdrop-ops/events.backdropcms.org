Views Bootstrap
---------------------

This module provides a set of styles for the Views module. It allows you to
create responsive Bootstrap 3.x components, such as grids, carousels, tabs, and tables,
all within the configuration settings of the powerful Views module.

IMPORTANT: If you are not using a Bootstrap theme, you must check
"Include Bootstrap CSS & JS when used by Views" on the configuration page for this module.

CONTENTS OF THIS FILE
---------------------

 - Introduction
 - Installation
 - Tested
 - Known Issues
 - Special Thanks
 - Requirements
 - Permissions
 - Coming From Drupal?
 - Usage
 - License
 - Credits
 - Maintainers

 INSTALLATION
------------

1) Install this module using the official Backdrop CMS instructions at https://backdropcms.org/guide/modules

2) Check config options at: admin/config/user-interface/views_bootstrap

TESTED
-----

The following components of this module have been tested to have a working Views configuration:
- Accordion
- Carousel
- Grid
- Media Object
- Tab
- Table
- Thumbnails

There are a lot of configurations that do not work due to incompatibility.  For example, making an accordion of one field does not work (these require a clickable title and hidden body content).  Making a table of one content teaser does not work (tables consist of fields in rows).

KNOWN ISSUES
---------------------

Some functionality may not be working. Please open an issue if you find anything that is broken or incomplete.

Knowledge of how to use different Views formats is required.  You may have to write CSS or JavaScript to make it look just like you want it to.

JavaScript is required for some components, and accessibility is not guaranteed.
Internet Explorer and older browsers are not supported.

To become familiar with what this module provides:

<http://getbootstrap.com/javascript/#carousel>  <http://getbootstrap.com/components>

This does not require a Bootstrap-based theme if you configure this module's settings correctly.

If you do not use a Bootstrap-based theme, then you can configure the Views Bootstrap module to load the library from a CDN at /admin/config/user-interface/views_bootstrap

SPECIAL THANKS
--------------

Sponsored by WikiJob <https://www.drupal.org/wikijob>

REQUIREMENTS
------------

Views module enabled.  Having a Bootstrap 3.x based theme helps, but there is a module setting for that in this version of the module.

PERMISSIONS
------------

none

COMING FROM DRUPAL
------------------

This module is very similar in functionality to its Drupal counterpart. However, the files have been reorganized a bit. Be sure to check the configuration page if you are not using a Bootstrap theme: admin/config/user-interface/views_bootstrap

USAGE
-----

Setup your options in the Views module.  Where you would once choose "List, Table"...now you can also choose "Bootstrap Carousel, Bootstrap Grid" etc...

### What is the difference between using a Bootstrap-based theme for your website and using this module?

Let's take the use-case of making a "Pinterest-style", responsive card style listing of content on your front page.  On large screens, you want it to be 4 columns wide with a sidebar on the right side.  On smaller screens, it should be 2 columns wide with a sidebar on the right side.  On a small screen, it should be 1 column wide with the sidebar underneath.  Using something like Bartik theme, you can install this module, select it, and it will happen, but the rest of the page will look like Bartik (or your theme).  If you were using something like Radix theme, the theme would look like Bootstrap but you would have to manually add the grid classes to your "cards" in your theme files to make the same effect.

LICENSE
-------

This project is GPL v2 software. See the LICENSE.txt file in this directory for complete text.

CREDITS
-----------

Initial development by Dmitry Demenchuk <http://www.demenchuk.me/>

MAINTAINERS
-----------

- Tim Erickson (https://github.com/stpaultim)
- David Radcliffe (https://github.com/radcliffe)

Ported to Backdrop by:

- biolithic <https://github.com/biolithic>
