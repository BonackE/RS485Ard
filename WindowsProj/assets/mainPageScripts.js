//read the select options as a string and return string
function GetOptions() {
    var select = document.getElementsByClassName("settingsLists");
    //create empty array to store the values
    var result = "";
    for (var i = 0; i < select.length; i++) {
        result += select[i].value;
        if(i != select.length - 1)
            result += ",";
    }
    return result;
}