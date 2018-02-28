;;;; This doesn't quite work, as the hash table seems to create a reference to the cell position, rather than a copy

(use-modules (ice-9 eval-string))

(define *cells* (make-hash-table))
;;(hash-create-handle! *cells* '(1 1) 42)
;;(hash-create-handle! *cells* '(1 1) 43)
;;(display (hash-get-handle *cells* '(1 1)))
;;(newline)


(define *current-cell* (cons 1 1))

(define (set-cell! cell-pos str)
  (hash-set! *cells* cell-pos str))

(define (get-cell cell-pos)
  (define cell-str (hash-ref *cells* cell-pos))
  ((eval-string cell-str)))

(define (get-curr-cell)
  (get-cell *current-cell*))


(define (curr-rc! str)
  (define full-str (string-append "(lambda () " str ")"))
  (set-cell! *current-cell* full-str))

(define ($r$c row col)
  (get-cell (cons row col)))

(define (eval-curr-cell)
  (get-cell *current-cell*))

(define (down-cell)
  (set-car! *current-cell* (1+ (car *current-cell*))))

(define (up-cell)
    (set-car! *current-cell* (1- (car *current-cell*))))

(define (println-curr-cell)
  (write (eval-curr-cell))
  (newline))

(curr-rc! "(+ 5 6)")
(println-curr-cell)
(down-cell)
(curr-rc! "(+ 10 11 ($r$c 1 1))")
(println-curr-cell)
(up-cell)
(println-curr-cell)

;;(goto-cell 1 1)
(define (desc) (hash-map->list cons *cells*))

