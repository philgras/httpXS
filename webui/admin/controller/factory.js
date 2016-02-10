httpXS.service("hostedServices",function(){
  var currentService = {}
  return{
    setServiceObj : function(obj){
    currentService = obj;
  },

  getServiceObj : function(){
    return currentService;
  }
};
});
