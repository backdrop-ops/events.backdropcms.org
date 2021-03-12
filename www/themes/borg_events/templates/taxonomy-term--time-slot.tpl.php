<?php
/**
 * @file
 * Time Slot.
 */
?>
<section class="taxonomy-term-<?php print $term->tid; ?> <?php print implode(' ', $classes); ?>">

  <?php if (!$page): ?>
    <h3><?php print $term_name; ?></h3>
  <?php endif; ?>

  <div class="content">
    <?php print render($content); ?>
  </div>

</section>
