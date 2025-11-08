#lang racket


(define *cells* (make-hash))


;(hash-set! the-cells '(1 2) 3)
;(hash-ref the-cells '(1 2))

(define (get-rc r c) (hash-ref *cells* `(,r ,c) null))

(define (set-rc! r c v) (hash-set! *cells* `(,r ,c) v))


(set-rc! 1 3 67)
(get-rc 1 3)
(get-rc 1 2)


(define (rng from to) (in-range from (+ 1 to)))

(define-syntax forn
  (syntax-rules ()
    ((_ var from to body ...)
     (for ([var (rng from to)])
       (begin body ...)))))

(forn i 1 10
      (display "i = ")
      (displayln i))
