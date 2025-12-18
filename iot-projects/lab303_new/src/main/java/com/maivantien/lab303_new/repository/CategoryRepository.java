package com.maivantien.lab303_new.repository;
import org.springframework.data.jpa.repository.JpaRepository;
import com.maivantien.lab303_new.entity.Category;
public interface CategoryRepository extends JpaRepository< Category, Long> {
}
