package com.maivantien.lab303_new.service;
import java.util.List;
import com.maivantien.lab303_new.entity.Category;
public interface CategoryService {
    Category createCategory(Category category);
    Category getCategoryById(Long categoryId);
    List<Category> getAllCategorys();
    Category updateCategory(Category category);
    void deleteCategory(Long categoryId);
}