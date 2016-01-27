
var selected = "";
var ip4_input= "<"

function setText(){

	document.getElementById("button").innerHTML = "Geklickt";

}

function addService(){

	var tab = document.getElementById("tServices");
	var el = document.createElement("tr");
	el.id = tab.childElementCount+"__row";
	el.innerHTML = "<td> <div class='serv-sym' style='background-color: #334499;'>#</div></td>" +
									"<td>tzu</td>" +
									"<td>ukz</td>" +
									"<td>jk</td>" +
									"<td>Yes</td>";

	el.addEventListener("click", function(){
		if(selected !== el.id){
				if(selected !== ""){
					document.getElementById(selected).classList.remove("bg-info");	
				}
				selected = el.id;
				el.classList.add("bg-info");
		}else{
				selected = "";
				el.classList.remove("bg-info");
		}
	});
	
	tab.appendChild(el);

}

function deleteService(){

	document.getElementById("tServices").removeChild(document.getElementById(selected));
	selected = "";

}


function calculateBannerColor(){

	var banner = document.getElementById("banner");
	if(banner === null) return;
	var colorStr = "#";
	for(var i = 0; i < 3; ++i){
		colorStr += Math.round(Math.random()* 99);
	}
	
	banner.style.backgroundColor = colorStr;

}

function addIP(){



s}



