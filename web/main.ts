const canvas = <HTMLCanvasElement>document.getElementById("canv");
const ctx = <CanvasRenderingContext2D>canvas.getContext("2d");
type WasmModule = {HEAP8: Uint8Array, _hi(): number, _solution(): number, calledRun: boolean};
const colors = ['red', 'green', 'blue', 'pink', 'cyan', 'gray', 'black', 'brown', 'orange','white','lime','purple','yellow','peru'];

function wasm(): WasmModule{
    return Module;
}

function from_cstr(Module: WasmModule, ptr: number){
    let str = "";
    while(Module.HEAP8[ptr] != 0){
        str += String.fromCharCode(Module.HEAP8[ptr]);
        ptr++;
    }
    return str;
}

class BallSort {
    balls: number[][];
    readonly widths: number[];
    readonly depth = 4;

    readonly r = {
        cWidth: 100,
        cHeight: 400,
        cInnerWidth: 80,
        cInnerHeight: 320,
        bRadius: 40 
    }

    constructor(count: number){
        this.widths = [Math.ceil(count/2), Math.floor(count/2)];
        
        this.balls = [];

        for(let i = 0; i < count; i++){
            this.balls.push(new Array(this.depth).fill(0))
        }
    }

    from_str(str: string){
        const grid = str.split('\n').map(s=>s.split(' '));
        for(let i = 0; i < grid[0].length; i++){
            for(let d = 0; d < this.depth; d++){
                const c = parseInt(grid[d][i]);
                this.balls[i][d] = c;
            }
        }
    }

    private draw_circle(ctx: CanvasRenderingContext2D, pos: [number, number], radius: number, color: string){
        ctx.beginPath();
        ctx.arc(pos[0], pos[1], radius, 0, 2 * Math.PI, false);
        ctx.fillStyle = color;
        ctx.fill();
        ctx.lineWidth = 5;
        ctx.strokeStyle = '#003300';
        ctx.stroke();
    }

    render(ctx: CanvasRenderingContext2D) {
        let i = 0;
        for(let h = 0; h < this.widths.length; h++){
            for(let c = 0; c < this.widths[h]; c++){
                let x = c * this.r.cWidth;
                let y = h * this.r.cHeight;
                
                let x1 = x + (this.r.cWidth - this.r.cInnerWidth) / 2;
                let x2 = x - (this.r.cWidth - this.r.cInnerWidth) / 2 + this.r.cWidth;

                let y1 = y + (this.r.cHeight - this.r.cInnerHeight) / 2;
                let y2 = y - (this.r.cHeight - this.r.cInnerHeight) / 2 + this.r.cHeight;

                ctx.beginPath();
                ctx.moveTo(x1,y1);
                ctx.lineTo(x1,y2);
                ctx.lineTo(x2,y2);
                ctx.lineTo(x2,y1);
                ctx.stroke();

                for(let b = 0; b < this.depth; b++){
                    let dy = this.r.bRadius * 2;
                    let xCenter = x + this.r.cWidth / 2;
                    let yCenter = y2 - this.r.bRadius;


                    if(this.balls[i][b] != -1){
                        this.draw_circle(ctx, [xCenter, yCenter - dy * (this.depth - b - 1)], this.r.bRadius, colors[this.balls[i][b]])
                    }
                }

                i++;
            }
        }
    }
}

let interval = setInterval(()=>{
    if(!wasm().calledRun){
        return;
    }

    clearInterval(interval);
    let puzzle = new BallSort(14);

    // let str = from_cstr(wasm(), wasm()._hi());
    
    let strs = from_cstr(wasm(), wasm()._solution()).split('|');

    let i = 0;

    let interval2 = setInterval(()=>{

        ctx.clearRect(0,0,1000,1000);

        puzzle.from_str(strs[i]);
    
        puzzle.render(ctx);

        i++;

        if(i == strs.length){
            clearInterval(interval2);
        }
    }, 500)

    
}, 100);
