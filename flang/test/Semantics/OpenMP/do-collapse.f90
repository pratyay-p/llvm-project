!RUN: %python %S/../test_errors.py %s %flang -fopenmp
! OpenMP Version 4.5
! 2.7.1 Collapse Clause
program omp_doCollapse
  integer:: i,j
  !ERROR: The value of the parameter in the COLLAPSE or ORDERED clause must not be larger than the number of nested loops following the construct.
  !$omp do collapse(3)
  do i = 1,10
    do j = 1, 10
      print *, "hello"
    end do
  end do
  !$omp end do

  do i = 1,10
    do j = 1, 10
      !ERROR: The value of the parameter in the COLLAPSE or ORDERED clause must not be larger than the number of nested loops following the construct.
      !$omp do collapse(2)
      do k = 1, 10
        print *, "hello"
      end do
      !$omp end do
    end do
  end do

  !$omp parallel do collapse(2)
    do i = 1, 3
      !ERROR: Loop control is not present in the DO LOOP
      !ERROR: The associated loop of a loop-associated directive cannot be a DO without control.
      do
      end do
    end do
end program omp_doCollapse

