/**
 * Documents the JS hooks.
 */

/**
 * The settings object stored here Drupal.settings.leaflet contains following
 * data:
 * - lMap: The Leaflet map object..
 * - mapId: The id of the map html object.
 * - map: Object with the original settings and leaflet instances:
 * -- layers: The configured layers.
 * -- lLayers: The leaflet layers created.
 * -- settings: The map settings.
 * -- description:
 * -- mapId: The id of the map html object.
 */

/**
 * Allows to adjust settings before the map is created.
 *
 * @param object event
 * @param object data
 *   Data to handle:
 *   - mapId: The id of the map html object.
 *   - settings: Settings from Drupal.settings.leaflet.
 */
$(document).bind('leaflet.init', function(event, data) {
});

/**
 * Allows to adjust the just prepared map object.
 *
 * @param object event
 * @param object data
 *   Data to handle:
 *   - lMap: Leaflet map object.
 *   - mapId: The id of the map html object
 *   - settings: Settings from Drupal.settings.leaflet.
 */
$(document).bind('leaflet.map_prepared', function(event, data) {
});

/**
 * Allows to adjust the layers before they are created.
 *
 * @param object event
 * @param object data
 *   Data to handle:
 *   - layers: The configured layers Map Object
 *   - lMap: Leaflet map object.
 *   - mapId: The id of the map html object
 *   - settings: Settings from Drupal.settings.leaflet.
 */
$(document).bind('leaflet.layers_perpare', function(event, data) {
});

/**
 * Allows to adjust a layer before it's created.
 *
 * @param object event
 * @param object data
 *   Data to handle:
 *   - layer: The layer to create.
 *   - key: The key of the layer.
 *
 * @return bool
 *   Return false to skip creating this layer.
 */
$(document).bind('leaflet.layer_create', function(event, data) {
  // Skip creating a layer.
  if (data.layer) {
    return false;
  }
});

/**
 * Allows to adjust a layer after it's created.
 *
 * @param object event
 * @param object data
 *   Data to handle:
 *   - lLayer: Leaflet Layer Object
 *   - layer: The layer to create.
 *   - key: The key of the layer.
 */
$(document).bind('leaflet.layer_alter', function(event, data) {
});

/**
 * Allows to adjust the layers after the standard processing is done.
 *
 * @param object event
 * @param object data
 *   Data to handle:
 *   - lLayers: Leaflet Layer Objects
 *   - lMap: Leaflet map object.
 *   - mapId: The id of the map html object
 *   - settings: Settings from Drupal.settings.leaflet.
 */
$(document).bind('leaflet.layers_alter', function(event, data) {
});

/**
 * Allows to adjust the features before they are created.
 *
 * @param object event
 * @param object data
 *   Data to handle:
 *   - features: Array with the configured features.
 *   - lMap: Leaflet map object.
 *   - mapId: The id of the map html object
 *   - settings: Settings from Drupal.settings.leaflet.
 */
$(document).bind('leaflet.features_prepare', function(event, data) {
});

/**
 * Allows to adjust a feature before it's created.
 *
 * @param object event
 * @param object data
 *   Data to handle:
 *   - feature: The feature configuration.
 *   - lMap: Leaflet map object.
 *
 * @return bool
 *   Return false to skip creating this feature.
 */
$(document).bind('leaflet.feature_create', function(event, data) {
  // Adjust values of the feature.
  data.feature.label = Drupal.t('Customized Feature Label');
  data.feature.lat =47.0099999;
  data.feature.lon =7.3308404;
});

/**
 * Allows to adjust a feature after it's created.
 *
 * @param object event
 * @param object data
 *   Data to handle:
 *   - lFeature: The Leaflet feature object.
 *   - feature: The feature configuration..
 *   - lMap: Leaflet map object.
 */
$(document).bind('leaflet.feature_alter', function(event, data) {
});

/**
 * Allows to adjust a feature after it's created.
 *
 * @deprecated in favor of leaflet.feature_alter
 */
$(document).bind('leaflet.feature', function(event, lFeature, feature) {
});

/**
 * Allows to adjust features after they are created.
 *
 * @param object event
 * @param object data
 *   Data to handle:
 *   - features: Array with the configured features.
 *   - lMap: Leaflet map object.
 *   - mapId: The id of the map html object
 *   - settings: Settings from Drupal.settings.leaflet.
 */
$(document).bind('leaflet.features_alter', function(event, data) {
});

/**
 * Allows to alter the whole map after it's processed.
 *
 * @param object event
 * @param object data
 *   Data to handle:
 *   - map: The settings object from Drupal.settings.leaflet.
 *   - lMap: Leaflet map object.
 *   - mapId: The id of the map html object
 *   - settings: Settings from Drupal.settings.leaflet.
 */
$(document).bind('leaflet.map_alter', function(event, data) {
});

/**
 * Allows to alter the whole map after it's processed.
 *
 * @deprecated in favor of leaflet.map_alter
 */
$(document).bind('leaflet.map', function(event, map, lMap) {
});
