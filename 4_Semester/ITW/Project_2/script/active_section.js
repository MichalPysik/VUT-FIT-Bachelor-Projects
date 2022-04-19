// ITW PROJEKT 2
// Autor: Michal Pyšík (login: xpysik00)

var current_section;
var last_id = "s1"; // defaultni sekce
var section_id = "s3"; // jen random value, prepise se


$(document).scroll(function () {

    $('.main_section').each(function () {

        if ($(this).position().top <= $(document).scrollTop() + 50 && ($(this).position().top + $(this).outerHeight()) > $(document).scrollTop()) {


            if ($(this).attr('id') == "zakladni_udaje") section_id = "s1";
            else if ($(this).attr('id') == "vzdelani") section_id = "s2";
            else if ($(this).attr('id') == "projekty") section_id = "s3";
            else if ($(this).attr('id') == "konicky") section_id = "s4";
            else section_id = "s5";

            current_section = document.getElementById(section_id);
            current_section.className = "active";

            if (last_id != section_id)
            {
                document.getElementById(last_id).className = "";
            }

            last_id = section_id;

        }

    });
});

