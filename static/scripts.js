// Google Map
let map;

// Markers for map
let markers = [];

// Info window
let info = new google.maps.InfoWindow();


// Execute when the DOM is fully loaded
$(document).ready(function() {

    // Styles for map
    // https://goo.gl/RXXb2
    let styles = [

        // Hide Google's labels
        {
            featureType: "all",
            elementType: "labels",
            stylers: [
                {visibility: "off"}
            ]
        },

        // Hide roads
        {
            featureType: "road",
            elementType: "geometry",
            stylers: [
                {visibility: "off"}
            ]
        }

    ];

    // Options for map
    // https://goo.gl/Y7D6Wz
    let options = {
        // New Haven
        center: {lat: 41.3184, lng: -72.9318},
        disableDefaultUI: true,
        mapTypeId: "terrain",
        maxZoom: 14,
        panControl: true,
        styles: styles,
        zoom: 13,
        zoomControl: true
    };

    // Get DOM node in which map will be instantiated
    let canvas = $("#map-canvas").get(0);
    // console.log(canvas);

    // Instantiate map
    map = new google.maps.Map(canvas, options);

    // Configure UI once Google Map is idle (i.e., loaded)
    google.maps.event.addListenerOnce(map, "idle", configure);

});


// Add marker for place to map
function addMarker(place)
{
    let pos = ({
        "lat": parseFloat(place["latitude"]),
        "lng": parseFloat(place["longitude"])
    });
    var marker = new google.maps.Marker({
        position: pos,
        map: map,
        title: place["place_name"],
        icon:{
            url: "http://maps.google.com/mapfiles/ms/micons/red.png",
            labelOrigin: new google.maps.Point(14,40)
        },
        label:{
            text: place["place_name"].toUpperCase() + "," + place["admin_code1"].toUpperCase(),
            fontWeight: "bold"
        },
        draggable:false
    });
    console.log(pos);


    var infowindow = new google.maps.InfoWindow({
        content: initinfo()
    });


    function initinfo(){
        var params = {q: place["postal_code"] };
        var results = $.getJSON("/articles", params, function(data, textStatus, jqXHR){
        text = "<h2>" + place["place_name"].toUpperCase() + "</h2>" +
        "<ul" + "id=\"" + place["postal_code"] + "\">";
        for (var i = 0; i < data[0].length; i++){
            result = data[0][i];
            text += "<li>" +
            "<a href=\"" + result["link"] + "\">" +
            result["title"] +
            "</a>" +
            "</li>"
        };
        text += "</ul>";
        infowindow.setContent(text);
    })
    };
    marker.addListener("click", function(){
            $("#q").blur();
            info.close();
            info = infowindow;
            info.open(map,marker);
    });

    marker.addListener("rightclick", function(event){
        console.log(event.latLng.lat(), event.latLng.lng());
    });

    markers.push(marker);

    //label1 = new google.maps.MarkerLabel

}


// Configure application
function configure()
{
    // Update UI after map has been dragged
    google.maps.event.addListener(map, "dragend", function() {

        // If info window isn't open
        // http://stackoverflow.com/a/12410385
        if (!info.getMap || !info.getMap())
        {
            update();
        }
    }
    );

    // Update UI after zoom level changes
    google.maps.event.addListener(map, "zoom_changed", function() {
        update();
    });

    // Configure typeahead
    // https://goo.gl/PziZcA


    var thisfunc = $("#q").typeahead(
        {
            highlight: false,
            minLength: 4
        },
        {
            display: function(suggestion) { return suggestion["postal_code"]},
            limit: 10,
            source: function(obj, sync, async){
                return search(obj, sync, async);
                },
            templates: {
                suggestion: Handlebars.compile("<div>" +
                    "{{{ iter this }}}"
                 + "</div>")
                }
        }

    );

    Handlebars.registerHelper("iter", function(obj){
                let entry = "";
                text = ("(" + obj["admin_code1"] + ") " + obj["place_name"].toUpperCase() + ", " + obj["admin_name2"].toUpperCase() + ", " + obj["admin_name3"]
                + "<br>" + "(" + obj["latitude"] + "," +  obj["longitude"]) + ")";
                // make a param list, then format that as text instead e.g ("admin_code1", "id"..)
                // * easier to check for key is null (i.e ignore it in text)
                text = text.replace(", None", "");
                entry = entry.concat(text);
                return entry;
            }
        );


    // Re-center map after place is selected from drop-down
    // typehead:select, function($eventObject, suggestionObject, dataset)
    $("#q").on("typeahead:select", function(eventObject, suggestion, name) {
        // Set map's center
        map.setCenter({lat: parseFloat(suggestion.latitude), lng: parseFloat(suggestion.longitude)});
        addMarker(suggestion);
        // Update UI
        update();
    })


    // Hide info window when text box has focus
    $("#q").focus(function(eventData) {
        info.close();
    });



    // Re-enable ctrl- and right-clicking (and thus Inspect Element) on Google Map
    // https://goo.gl/NkbM6
    document.addEventListener("contextmenu", function(event) {
        if (event.target.title){
            // idk how to copy text/ coordinates to clipboard
        }
        else{
            event.returnValue = true;
            event.stopPropagation && event.stopPropagation();
            event.cancelBubble && event.cancelBubble();
        }

    }, true);

    // Update UI
    update();

    // Give focus to text box
    // http://api.jquery.com/focus/
    $("#q").focus();
}


// Remove markers from map
function removeMarkers()
{
    for (var i = 0; i < markers.length; i++){
        markers[i].setMap(null);

    };
    markers = [];
}


// Search database for typeahead's suggestions

function search(query, syncResults, asyncResults)
{
    // Get places matching query (asynchronously)
    let params = { q: query };
    var thisfunc = $.getJSON("/search",
                      params, function(data, textStatus, jqXHR){
                                    if (!data.length){
                                        data = [data];}
                                    asyncResults(data);
                                }
                    );


}


// Show info window at marker with content
function showInfo(marker, content)
{
    // Start div
    let div = "<div id='info'>";
    if (typeof(content) == "undefined")
    {
        // http://www.ajaxload.info/
        div += "<img alt='loading' src='/static/ajax-loader.gif'/>";
    }
    else
    {
        div += content;
    }

    // End div
    div += "</div>";

    // Set info window's content
    info.setContent(div);
    print(div)

    // Open info window (if not already open)
    info.open(map, marker);
}


// Update UI's markers
function update()
{
    if ($("#q").val() == ""){
        // ignore if typeahead box is empty
    }

    // Get map's bounds
    let bounds = map.getBounds();
    let ne = bounds.getNorthEast();
    let sw = bounds.getSouthWest();

    // Get places within bounds (asynchronously)
    let parameters = {
        ne: `${ne.lat()},${ne.lng()}`,
        q: $("#q").val(),
        sw: `${sw.lat()},${sw.lng()}`
    };
    $.getJSON("/update", parameters, function(data, textStatus, jqXHR) {

       // Remove old markers from map
       removeMarkers();

       // Add new markers to map
       console.log(data);
       for (let i = 0; i < data.length; i++)
       {
           addMarker(data[i]);
       }
    });
};
