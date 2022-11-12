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
  $(".BtnIrrigarManual").addClass("fa-cloud-sun");
  $(".BtnIrrigarManual").removeClass("fa-cloud-sun-rain");
  var database = firebase.database();
  var IrrigarAutomatico, IrrigarManual, UmidadeAtual, Ciclos, Historico;

  database.ref().on("value", function (snap) {
    IrrigarAutomatico = snap.val().irrigarAutomatico;
    IrrigarManual = snap.val().irrigarManual;
    UmidadeAtual = snap.val().umidadeAtual;
    Historico = snap.val().historico;
    Ciclos = snap.val().ciclos;

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
      console.log(element);
      $(".listagem-historico").append(linha);
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

  function toggleBtnIrrigar(valor) {
    console.log(valor);
  }
});
