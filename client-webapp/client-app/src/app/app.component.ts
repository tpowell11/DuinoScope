//THIS IS THE EQUIVALENT OF main.c
import { Component } from '@angular/core';
class CameraImageStream{
  width:number
  height:number
  constructor(resY:number, resX:number){
    this.width = resX
    this.height = resY
  }
  function getImage(path:string) {
    
  }
}
let ImgStream = new CameraImageStream(320, 640); 
@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.css']
})

export class AppComponent {
  title = 'client-app';
}
