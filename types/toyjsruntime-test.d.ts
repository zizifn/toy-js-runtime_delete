declare module 'toyjsruntime:test' {
    /**
     * Calculates the Fibonacci number at the given position
     * @param n The position in the Fibonacci sequence
     * @returns The Fibonacci number at position n
     */
    export function fib(n: number): number;
}

declare module 'toyjsruntime:net' {

    export function fetch(url: string): Promise<Response>;
}

declare module 'toyjsruntime:jsmodules' {
    export interface TestObject {
        obj1: string;
    }
    export const obj: TestObject;
    export function testJSfunction(): string;
}