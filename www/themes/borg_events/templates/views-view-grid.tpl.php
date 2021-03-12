<?php
/**
 * @file
 * Views grid.
 * - overrides H3 for group heading.
 */
?>
<div class="group">
  <?php if (!empty($title)) : ?>
    <div class="group-head"><?php print $title; ?></div>
  <?php endif; ?>

  <?php if (empty($deprecated_table)) : ?>
  <div class="<?php print implode(' ', $classes); ?>"<?php print backdrop_attributes($attributes); ?>>
    <?php foreach ($rows as $row_count => $row): ?>
      <div <?php if (!empty($row_classes[$row_count])) { print 'class="' . implode(' ', $row_classes[$row_count]) . '"';  } ?>>
        <?php print $row; ?>
      </div>
    <?php endforeach; ?>
  </div>
  <?php endif; ?>

  <?php if (!empty($deprecated_table)) : ?>
  <table class="<?php print implode(' ', $classes); ?>"<?php print backdrop_attributes($attributes); ?>>
    <?php if (!empty($caption)) : ?>
      <caption><?php print $caption; ?></caption>
    <?php endif; ?>

    <tbody>
      <?php foreach ($rows as $row_number => $columns): ?>
        <tr <?php if (!empty($row_classes[$row_number])) { print 'class="' . implode(' ', $row_classes[$row_number]) .'"';  } ?>>
          <?php foreach ($columns as $column_number => $item): ?>
            <td <?php if ($column_classes[$row_number][$column_number]) { print 'class="' . implode(' ', $column_classes[$row_number][$column_number]) .'"';  } ?>>
              <?php print $item; ?>
            </td>
          <?php endforeach; ?>
        </tr>
      <?php endforeach; ?>
    </tbody>
  </table>
  <?php endif; ?>
</div>
