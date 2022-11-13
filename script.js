function controlFromInput(fromSlider, fromInput, toInput, controlSlider) {
  const [from, to] = getParsed(fromInput, toInput);
  fillSlider(fromInput, toInput, "#C6C6C6", "#25daa5", controlSlider);
  if (from > to) {
    fromSlider.value = to;
    fromInput.value = to;
  } else {
    fromSlider.value = from;
  }
}

function controlToInput(toSlider, fromInput, toInput, controlSlider) {
  const [from, to] = getParsed(fromInput, toInput);
  fillSlider(fromInput, toInput, "#C6C6C6", "#25daa5", controlSlider);
  setToggleAccessible(toInput);
  if (from <= to) {
    toSlider.value = to;
    toInput.value = to;
  } else {
    toInput.value = from;
  }
}

function controlFromSlider(fromSlider, toSlider, fromInput) {
  const [from, to] = getParsed(fromSlider, toSlider);
  fillSlider(fromSlider, toSlider, "#C6C6C6", "#25daa5", toSlider);
  if (from > to) {
    fromSlider.value = to;
    fromInput.value = to;
  } else {
    fromInput.value = from;
  }
}

function controlToSlider(fromSlider, toSlider, toInput) {
  const [from, to] = getParsed(fromSlider, toSlider);
  fillSlider(fromSlider, toSlider, "#C6C6C6", "#25daa5", toSlider);
  setToggleAccessible(toSlider);
  if (from <= to) {
    toSlider.value = to;
    toInput.value = to;
  } else {
    toInput.value = from;
    toSlider.value = from;
  }
}

function getParsed(currentFrom, currentTo) {
  const from = parseInt(currentFrom.value, 10);
  const to = parseInt(currentTo.value, 10);
  return [from, to];
}

function fillSlider(from, to, sliderColor, rangeColor, controlSlider) {
  const rangeDistance = to.max - to.min;
  const fromPosition = from.value - to.min;
  const toPosition = to.value - to.min;
  controlSlider.style.background = `linear-gradient(
    to right,
    ${sliderColor} 0%,
    ${sliderColor} ${(fromPosition / rangeDistance) * 100}%,
    ${rangeColor} ${(fromPosition / rangeDistance) * 100}%,
    ${rangeColor} ${(toPosition / rangeDistance) * 100}%, 
    ${sliderColor} ${(toPosition / rangeDistance) * 100}%, 
    ${sliderColor} 100%)`;
}

function setToggleAccessible(currentTarget) {
  const toSlider = document.querySelector("#toSlider");
  if (Number(currentTarget.value) <= 0) {
    toSlider.style.zIndex = 2;
  } else {
    toSlider.style.zIndex = 0;
  }
}

var from = 10;
var to = 10;
const fromSlider = document.querySelector("#fromSlider");
const toSlider = document.querySelector("#toSlider");
const fromInput = document.querySelector("#fromInput");
const toInput = document.querySelector("#toInput");
fillSlider(fromSlider, toSlider, "#C6C6C6", "#25daa5", toSlider);
setToggleAccessible(toSlider);

fromSlider.oninput = () => controlFromSlider(fromSlider, toSlider, fromInput);
toSlider.oninput = () => controlToSlider(fromSlider, toSlider, toInput);
from = controlFromInput(fromSlider, fromInput, toInput, toSlider);
to = controlToInput(toSlider, fromInput, toInput, toSlider);

const firebaseConfig = {
  apiKey: "AIzaSyDQtxvT_mEQGcojyFJCWlyFCf623vBrUkw",
  authDomain: "horta-vertical-96557.firebaseapp.com",
  databaseURL: "https://horta-vertical-96557-default-rtdb.firebaseio.com",
  projectId: "horta-vertical-96557",
  storageBucket: "horta-vertical-96557.appspot.com",
  messagingSenderId: "400541554813",
  appId: "1:400541554813:web:fe56a2b477e904f33c7914",
  measurementId: "G-KCMRTRP9D6",
};
// Initialize Firebase
firebase.initializeApp(firebaseConfig);

$(document).ready(function () {
  $(".listagem-historico").html("Loading...");
  $(".BtnIrrigarManual").addClass("fa-cloud-sun");
  $(".BtnIrrigarManual").removeClass("fa-cloud-sun-rain");
  var database = firebase.database();
  var IrrigarAutomatico,
    IrrigarManual,
    UmidadeAtual,
    Ciclos,
    Historico,
    Range,
    qtdLista;

  qtdLista = 0;

  database.ref().on("value", function (snap) {
    IrrigarAutomatico = snap.val().irrigarAutomatico;
    IrrigarManual = snap.val().irrigarManual;
    UmidadeAtual = snap.val().umidadeAtual;
    Historico = snap.val().historico;
    Ciclos = snap.val().ciclos;
    Range = snap.val().range;

    if (IrrigarAutomatico == "1") {
      document.getElementById("unact").style.display = "none";
      document.getElementById("act").style.display = "block";
    } else {
      document.getElementById("unact").style.display = "block";
      document.getElementById("act").style.display = "none";
    }

    if (IrrigarManual == "1") {
      $(".BtnIrrigarManual").addClass("fa-cloud-sun-rain");
      $(".BtnIrrigarManual").removeClass("fa-cloud-sun");
    } else {
      $(".BtnIrrigarManual").addClass("fa-cloud-sun");
      $(".BtnIrrigarManual").removeClass("fa-cloud-sun-rain");
    }

    $(".status-umidade").text(UmidadeAtual + "%");
    $(".appbar__controls-right-span").text(Ciclos);

    $(".listagem-historico").html("");
    Historico.forEach((element) => {
      var linha =
        "<a class='list__item'><i class='fa-solid fa-2x fa-shower'></i><div class='list__details'>" +
        "<h2>" +
        moment(element.data).format("DD/MM/YYYY") +
        "</h2>" +
        "<p><i class=fa-solid fa-temperature-arrow-down'></i>Inicio " +
        element.inicio +
        "% |" +
        "<i class='fa-solid fa-temperature-arrow-up'></i> Termino " +
        element.termino +
        "% | " +
        "<i class='fa-solid fa-plug'></i> Tempo " +
        element.tempo +
        " min</p></div></a>";

      $(".listagem-historico").append(linha);
      qtdLista = Historico.lenght;

      fromSlider.value = Range.min;
      toSlider.value = Range.max;
      fromInput.value = Range.min;
      toInput.value = Range.max;
      $(".min-irrigar").text(Range.min + "%");
      $(".max-irrigar").text(Range.max + "%");
    });
  });

  $(".toggle-btn").click(function () {
    var firebaseRef = firebase.database().ref().child("irrigarAutomatico");
    if (IrrigarAutomatico == "1") {
      firebaseRef.set("0");
      IrrigarAutomatico = "0";
    } else {
      firebaseRef.set("1");
      IrrigarAutomatico = "1";
    }
  });

  $(".BtnIrrigarManual").click(function () {
    var firebaseRef1 = firebase.database().ref().child("irrigarManual");
    if (IrrigarManual == "1") {
      $(".BtnIrrigarManual").addClass("fa-cloud-sun");
      $(".BtnIrrigarManual").removeClass("fa-cloud-sun-rain");
      firebaseRef1.set("0");
      IrrigarManual = "0";
    } else {
      $(".BtnIrrigarManual").addClass("fa-cloud-sun-rain");
      $(".BtnIrrigarManual").removeClass("fa-cloud-sun");
      firebaseRef1.set("1");
      IrrigarManual = "1";
    }
  });

  $("#fromSlider").change(function () {
    var range = {
      min: fromInput.value,
      max: toInput.value,
    };
    var firebaseRef = firebase.database().ref().child("range");
    firebaseRef.set(range);
    $(".min-irrigar").text(range.min + "%");
    $(".max-irrigar").text(range.max + "%");
  });
  $("#toSlider").change(function () {
    var range = {
      min: fromInput.value,
      max: toInput.value,
    };
    var firebaseRef = firebase.database().ref().child("range");
    firebaseRef.set(range);
    $(".min-irrigar").text(range.min + "%");
    $(".max-irrigar").text(range.max + "%");
  });
});
