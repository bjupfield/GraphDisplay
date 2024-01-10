const MyImage = document.querySelector(".MyImage")
const myHtml = document.querySelector("html")
MyImage.src = "s.jpg"
//MyImage.heigh
MyImage.height = 250;
MyImage.width = 250;
MyImage.onclick = b;
window.onresize = resize;

const ResizeList = document.getElementsByClassName("resize")


function b(){
    var mySrc = this.src
    if(mySrc.toString().endsWith("b.jpg"))
    {
        this.src = "s.jpg"
    }
    else
    {
        console.log(this.src)
        this.src = "b.jpg"
    }
}
function resize(){
    console.log(ResizeList.length)
    //console.log("ClientWidth: " + this.innerWidth + " ||| ClientHeight: " + this.innerHeight)
}