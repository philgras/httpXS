httpXS.controller('createCtrl',['$scope','$uibModalInstance',function($scope,$uibModalInstance){
  console.log("create");

  $scope.calculateBannerColor = function (){

  	var banner = document.getElementById("banner");
  	if(banner === null) return;
  	var colorStr = "#";
  	for(var i = 0; i < 3; ++i){
  		colorStr += Math.round(Math.random()* 99);
  	}

  	banner.style.backgroundColor = colorStr;

  };

  $scope.calculateBannerColor();
}]);
