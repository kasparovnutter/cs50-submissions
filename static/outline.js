
let map;
let markers = [];
let info = new google.maps.InfoWindow();

$(document).ready(
function()
    {
        // init styles                          (https://goo.gl/RXXb2)
        // init options                         (https://goo.gl/zybh30)
        let canvas = $("#map-canvas").get(0);
        map = new google.maps.Map(canvas, options);
        google.maps.event.addListenerOnce(map, "idle", configure);
    }
)

function addMarker(place)
{
    // TODO
}

function removeMarker(place)
{
    // TODO
}

function configure()
{

}