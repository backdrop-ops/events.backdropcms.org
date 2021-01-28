<?php if (!empty($title)): ?>
  <h3><?php print $title ?></h3>
<?php endif ?>

<div id="views-bootstrap-carousel-<?php print $id ?>" data-ride="carousel" class="carousel slide <?php foreach($classes as $key => $value) { print $value; } ?>">
  <?php if ($indicators): ?>
    <!-- Carousel indicators -->
    <ol class="carousel-indicators">
      <?php foreach ($rows as $key => $value): ?>
        <li data-target="#views-bootstrap-carousel-<?php print $id ?>" data-slide-to="<?php print $key ?>" class="<?php if ($key == $first_key) print 'active' ?>"></li>
      <?php endforeach ?>
    </ol>
  <?php endif ?>

  <!-- Carousel items -->
  <div class="carousel-inner" role="listbox">
    <?php foreach ($rows as $key => $row): ?>
      <div class="item <?php if ($key == $first_key) print 'active' ?>">
        <?php print $row ?>
      </div>
    <?php endforeach ?>
  </div>

  <?php $id_prev = $id - 1; $id_next = $id + 1; if ($navigation): ?>
    <!-- Carousel navigation -->
    <a class="left carousel-control" href="#views-bootstrap-carousel-<?php print $id ?>" data-slide="prev" role="button">
      <span class="icon-prev"></span>
    </a>
    <a class="right carousel-control" href="#views-bootstrap-carousel-<?php print $id ?>" data-slide="next" role="button">
      <span class="icon-next"></span>
    </a>
  <?php endif ?>
</div>
